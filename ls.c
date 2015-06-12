#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>	
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

size_t a_flag;
size_t R_flag;
size_t l_flag;
size_t dir_flag;

void ls_dir(char* path);
void print_type(mode_t* m);
void print_stats(struct stat* f_st, char* name);
void passed_dir(int argc, char** argv);
size_t protect(char* n);

int main(int argc, char** argv) {
	int opt;
	size_t i;
	while ((opt = getopt(argc, argv, "alR")) != -1) {
		if(!a_flag)
			a_flag = opt == 'a' ? 1 : 0;
		if(!l_flag)
			l_flag = opt == 'l' ? 1 : 0;
		if(!R_flag)
			R_flag = opt == 'R' ? 1 : 0;
    }
    passed_dir(argc, argv);
	if(!dir_flag)
		ls_dir(".");
	for(i = 1;i < argc;++i) {
		if(argv[i][0] != '-')
			ls_dir(argv[i]);
	}
	return 0;
}

void ls_dir(char* path) { 
	DIR* dir = opendir(path);
	if(!dir)
		return;
	struct dirent* ent = readdir(dir);
	size_t flag, i;
	struct stat f_stat;
	char* base = (char*) malloc(1 * sizeof(char));
	char* dash = "/";
	size_t dir_curr_size = 1;
	char** dirs = (char**) malloc(dir_curr_size * sizeof(char*));
	size_t dir_count = 0;
	if(dir_flag)
		printf("%s:\n", path);
	while(ent) {
		flag = ent->d_name[0] == '.' ? 1 : 0;
		if(!flag || (flag && a_flag)) {
			base = (char*) realloc(base, sizeof(path) + sizeof(ent->d_name) + 1);
			base = strcpy(base, path);
			base = strcat(base, dash);
			base = strcat(base, ent->d_name);
			stat(base, &f_stat);
			print_type(&f_stat.st_mode);
			l_flag ? print_stats(&f_stat, ent->d_name) : printf(" %s\n", ent->d_name);
			if(protect(ent->d_name) && (R_flag && (S_ISDIR(f_stat.st_mode)))) {
				if(dir_count == dir_curr_size) 
					dirs = (char**) realloc(dirs, ++dir_curr_size * sizeof(char*));
				dirs[dir_count] = (char *) malloc(strlen(base) + 1 * sizeof(char));
				strcpy(dirs[dir_count], base);
				dir_count++;
			}
		}
		ent = readdir(dir);
	}
	if(R_flag) {
		for(i =0; i < dir_count;++i) {
			ls_dir(dirs[i]);
			free(dirs[i]);
		}
	}
	closedir(dir);
	free(dirs);
	free(base);
}

void print_type(mode_t* m) {
	
	printf("%c", S_ISREG(*m) ? '-' : S_ISDIR(*m) ? 'd' : S_ISBLK(*m) ? 'b' :
	S_ISCHR(*m) ? 'c' : S_ISFIFO(*m) ? 'p' : S_ISLNK(*m) ? 'l' : S_ISSOCK(*m) ? 's' : ' ');
}

void print_stats(struct stat* f_st, char* name) {
	mode_t m = f_st->st_mode;
	struct passwd* user;
	struct group* grp;
	struct tm* t;
	long long int size = (long long int) f_st->st_size;
	long long int links = (long long int) f_st->st_nlink;
	size_t max = 200;
	char date[max];
	user = getpwuid(f_st->st_uid);
	user = getpwuid(f_st->st_uid);
	grp = getgrgid(f_st->st_gid); 
	t = gmtime(&f_st->st_mtime);
	strftime(date, sizeof(date), "%b %d %R", t);
	printf("%c%c%c%c%c%c%c%c%c %lld %s %s %lld %s %s\n",
	m & S_IRUSR ? 'r' : '-', m & S_IWUSR ? 'w' : '-',  m & S_IXUSR ? 'x' : '-',  m & S_IRGRP ? 'r' : '-',  m & S_IWGRP ? 'w' : '-',
	m & S_IXGRP ? 'x' : '-', m & S_IROTH ? 'r' : '-',  m & S_IWOTH ? 'w' : '-',  m & S_IXOTH ? 'x' : '-', links,
	user->pw_name, grp->gr_name, size, date, name);  
}

void passed_dir(int argc, char** argv) {
	size_t count = 0;
	size_t i;
	for(i = 1;i < argc;++i) {
		if(argv[i][0] != '-')
			count++;
	}
	dir_flag = !count ? 0 : 1;
}

size_t protect(char* name) {
	if((strlen(name) == 1) && (name[0] == '.'))
		return 0;
	if((name[0] == '.') && (name[1] == '.'))
		return 0;
	return 1;
}