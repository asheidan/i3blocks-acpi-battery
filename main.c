#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

long battery_data(const char *path) {
	const size_t buf_size = 64;
	char buffer[buf_size];
	long result = 0;

	int fd = open(path, 0);
	if (0 > fd) {
		perror("open");
		return -1;
	}
	ssize_t size_read = read(fd, buffer, buf_size);
	if (0 > size_read) {
		perror("read");
		close(fd);
		return 0;
	}
	close(fd);

	if (0 < size_read) {
		errno = 0;
		result = strtol(buffer, NULL, 10);
		if (0 != errno) {
			perror("strtol");
		}
	}

	return result;
}

char battery_status(const char *path) {
	char buffer;
	int fd = open(path, 0);
	if (0 > fd) {
		perror("open");
		return -1;
	}
	ssize_t size_read = read(fd, &buffer, sizeof(buffer));
	if (0 > size_read) {
		perror("read");
		close(fd);
		return 0;
	}
	close(fd);

	return buffer;

}


int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
	int error = chdir("/sys/class/power_supply");
	if (0 > error) {
		perror("chdir");
		return -1;
	}

	long charge_now = battery_data("BATC/charge_now");
	long charge_full = battery_data("BATC/charge_full");
	long current_now = battery_data("BATC/current_now");

	/*
	fprintf(stdout, "charge: %ld\n", charge_now);
	fprintf(stdout, "full: %ld\n", charge_full);
	fprintf(stdout, "%ld%%\n", (charge_now * 100) / charge_full);
	fprintf(stdout, "current: %ld\n", current_now);
	*/

	char status = battery_status("BATC/status");
	long charge_percent = (charge_now * 100) / charge_full;

	char *status_word = "???";
	switch (status) {
		case 'D':
			status_word = "BAT";
			break;
		case 'C':
			status_word = "CHR";
			break;
		case 'N':
			status_word = "Not";
			break;
	}

	fprintf(stdout, " %s %3ld%% ", status_word, charge_percent);

	long charge_remaining;
	if ('D' == status) {
		/* status is "Discharging" */
		charge_remaining = charge_now;
	}
	else {
   		charge_remaining = charge_full - charge_now;
	}

	if ('N' != status && 0 < charge_remaining) {
		int hours = charge_remaining / current_now;
		charge_remaining = (charge_remaining % current_now) * 60;
		int minutes = charge_remaining / current_now;
		/* Add an extra minute if theres more than 30 seconds left and we're charging */
		if ('C' == status) {
			minutes += (charge_remaining % current_now) * 60 / current_now > 30;
		}
		/*
		charge_remaining = (charge_remaining % current_now) * 60;
		int seconds = charge_remaining / current_now;
		*/

		fprintf(stdout, "%d:%02d ", hours, minutes);
	}
	else {
		fprintf(stdout, "-:-- ");
	}

	putc('\n', stdout);

	return ((charge_percent < 20) && ('D' == status)) ? 33 : 0;
}
