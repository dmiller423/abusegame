/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 2001 Anthony Kruize <trandor@labyrinth.net.au>
 *  Copyright (c) 2005-2011 Sam Hocevar <sam@hocevar.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 */

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#ifdef WIN32
# include <Windows.h>
#elif !defined(PS4)
# include <dirent.h>
#else
#include <string>
#include <vector>
#include <kernel.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef PS4

using namespace std;

struct Dentry {
	std::string name;
	uint32_t type;
};

bool getEntries(const std::string& dir, std::vector<Dentry>& entries, bool wantDots)
{
	printf("%s(\"%s\")\n",__func__,dir.c_str());

	static std::vector<char> dentBuff(0x10000,0);


	int dfd = open(dir.c_str(), O_RDONLY | O_DIRECTORY);
	if (dfd > 0) {

		dentBuff.resize(0x10000);	// 64k ought to be enough, really... 

		int dsize = 0;
		while (0 < (dsize = sceKernelGetdents(dfd, &dentBuff[0], dentBuff.size())))
		{
			int offs = 0;
			dirent *de = (dirent *)&dentBuff[offs];

			while (offs < dsize && de) {
				de = (dirent *)&dentBuff[offs];

				entries.push_back(Dentry{ std::string(de->d_name), de->d_type });

				printf("$$$ entry fileNo: 0x%08X , type: %jo , name: \"%s\" \n", de->d_fileno, de->d_type, de->d_name);

				offs += de->d_reclen;
			}
		}

		close(dfd);

	}
	else return false;

	return true;
}

bool getEntries(const char* dir, std::vector<Dentry>& entries, bool wantDots)
{
	string sDir(dir);
	return getEntries(sDir, entries, wantDots);
}

void debugListApp0()
{
	vector<Dentry> dents;
	if (!getEntries("/app0", dents, true))
		printf("ERROR: %s() failed!\n",__func__);
}
#endif

void get_directory(char *path, char **&files, int &tfiles, char **&dirs, int &tdirs)
{
    struct dirent *de;
    files = NULL;
    dirs = NULL;
    tfiles = 0;
    tdirs = 0;
#ifdef WIN32
	WIN32_FIND_DATA findData;
	HANDLE d = FindFirstFile(path, &findData);
	if (d == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			tdirs++;
			dirs = (char **)realloc(dirs, sizeof(char *)*tdirs);
			dirs[tdirs - 1] = strdup(findData.cFileName);
		}
		else
		{
			tfiles++;
			files = (char **)realloc(files, sizeof(char *)*tfiles);
			files[tfiles - 1] = strdup(findData.cFileName);
		}
	} while( FindNextFile(d, &findData) );
	FindClose( d );
#elif defined(PS4)

	//int _d = open(path)

	//klog("---------------------- %s() needs *FIXME*\n", __func__);

	static std::vector<char> dentBuff(0x10000,0);


	int dfd = open(path, O_RDONLY | O_DIRECTORY);
	if (dfd > 0) {

		dentBuff.resize(0x10000);	// 64k ought to be enough, really... 

		int dsize = 0;
		while (0 < (dsize = sceKernelGetdents(dfd, &dentBuff[0], dentBuff.size())))
		{
			int offs = 0;
			dirent *de = (dirent *)&dentBuff[offs];

			while (offs < dsize && de) {
				de = (dirent *)&dentBuff[offs];

				if (DT_DIR==de->d_type) {
					tdirs++;
					dirs = (char **)realloc(dirs, sizeof(char *)*tdirs);
					dirs[tdirs - 1] = strdup(de->d_name);
				}
				else
				{
					tfiles++;
					files = (char **)realloc(files, sizeof(char *)*tfiles);
					files[tfiles - 1] = strdup(de->d_name);
				}

				printf("$$$ entry fileNo: 0x%08X , type: %jo , name: \"%s\" \n", de->d_fileno, de->d_type, de->d_name);

				offs += de->d_reclen;
			}
		}

		close(dfd);

	}
#else
    DIR *d = opendir( path );

    if( !d )
        return;

    char **tlist = NULL;
    int t = 0;
    char curdir[200];
    getcwd( curdir, 200 );
    chdir( path );

    do
    {
        de = readdir( d );
        if( de )
        {
            t++;
            tlist = (char **)realloc(tlist,sizeof(char *)*t);
            tlist[t-1] = strdup(de->d_name);
        }
    } while( de );
    closedir( d );

    for( int i=0; i < t; i++ )
    {
        d = opendir( tlist[i] );
        if( d )
        {
            tdirs++;
            dirs = (char **)realloc(dirs,sizeof(char *)*tdirs);
            dirs[tdirs-1] = strdup(tlist[i]);
            closedir( d );
        }
        else
        {
            tfiles++;
            files = (char **)realloc(files,sizeof(char *)*tfiles);
            files[tfiles-1] = strdup(tlist[i]);
        }
        free( tlist[i] );
    }
    if( t )
        free( tlist );
    chdir( curdir );
#endif
}
