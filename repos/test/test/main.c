#define _CRT_SECURE_NO_WARNINGS 1

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char* argv[]) {

	//if (argc != 2) return 1;

	int desc = open("balt.txt", O_WRONLY | O_CREAT | O_TRUNC, 0400);
	if (desc < 0) {
		perror("Erreur d'ouverture");
		return 1;
	}

	// Suite de 10 lettres random
	srand(time(0));
	char rands[11];
	for (int i = 0; i < 10; i++) rands[i] = rand() % 26 + 97;
	rands[10] = '\0';

	// Date et heure
	time_t now;
	time(&now);
	struct tm* info = localtime(&now);

	char date[13];
	char heure[7];
	strftime(date, sizeof(date), "%d%m%y", info);
	strftime(heure, sizeof(heure), "%H%M%S", info);

	char uid[4];
	sprintf(uid, "%d", 21);

	char* buffer = "<AG_new_user>\n\t<AG_date>\n\t\t<AG_day>%s</AG_day>\n\t\t<AG_hour>%s</AG_hour>\n\t</AG_date>\n\t<AG_name>%s</AG_name>\n\t<AG_UID>%d</AG_UID>\n\t<AG_password>%s</AG_password>\n</AG_new_user>\n";

	printf(buffer, date, heure, "d", 21, rands);
	write(desc, buffer, 35);
	write(desc, date, strlen(date));
	buffer += 37;
	write(desc, buffer, 21);
	write(desc, heure, strlen(heure));
	buffer += 23;
	write(desc, buffer, 33);
	write(desc, "root", strlen("root"));
	buffer += 35;
	write(desc, buffer, 20);
	write(desc, uid, strlen(uid));
	buffer += 22;
	write(desc, buffer, 24);
	write(desc, rands, strlen(rands));
	buffer += 26;
	write(desc, buffer, 30);

	close(desc);

	return 0;
}
