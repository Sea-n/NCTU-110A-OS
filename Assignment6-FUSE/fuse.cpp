#define FUSE_USE_VERSION 30
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <fuse.h>
#include <map>

using namespace std;

/*
 * Student ID: 0816146
 * Student Name: Sean Wei
 * Email: nctu-os@sean.taipei
 * SE tag: xnxcxtxuxoxsx
 * Statement: I am fully aware that this program is not supposed to be posted
 * to a public server, such as a public GitHub repository or a public web page.
 */

int my_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
int my_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi);
int my_getattr(const char *path, struct stat *st);
static struct fuse_operations op;

struct Attr {
	int uid, gid;
	string path;
	size_t size;
	bool is_dir;
	int mtime;
	int mode;
	int pos;
};

struct tar_header {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char typeflag[1];
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char pad[12];
};

/* Open file in global scope :) */
ifstream tar("test.tar", ios::binary);
map<size_t, vector<string>> dir;
map<size_t, Attr *> attr;
hash<string> hasher;

const string ROOT = "/";

int main(int argc, char** argv) {
	if (!tar) {
		cerr << "Cannot open test.tar.\n";
		return 1;
	}

	/* Read tar to my own structure */
	while (true) {
		tar_header *hdr = new tar_header;
		Attr *a = new Attr;

		tar.read((char *) hdr, 512);

		if (hdr->name[0] == '\0')
			break;

		hdr->size[11] = hdr->mtime[11] = '\0';

		a->mtime = stoi(hdr->mtime, 0, 8);
		a->mode = stoi(hdr->mode, 0, 8) | (hdr->typeflag[0] == '5' ? S_IFDIR : S_IFREG);
		a->size = stoi(hdr->size, 0, 8);
		a->uid = stoi(hdr->uid, 0, 8);
		a->gid = stoi(hdr->gid, 0, 8);
		a->path = ROOT + hdr->name;
		a->pos = tar.tellg();
		if (a->path.back() == '/') a->path.pop_back();  // remove trailing-slash for directory

		tar.seekg((a->size + 511) & ~511, ios::cur);

		/* Find repeated name, keep newer one */
		auto it = attr.find(hasher(a->path));
		if (it != attr.end()) {
			if (it->second->mtime > a->mtime)
				continue;  // Existing one is newer, not updating
		}

		attr[hasher(a->path)] = a;
	}

	/* directory listing */
	for (auto &it : attr) {
		int pos = it.second->path.find_last_of('/', it.second->path.size() - 2);
		string par = it.second->path.substr(0, pos + 1);
		string sub = it.second->path.substr(pos + 1);
		dir[hasher(par)].push_back(sub);
	}

	/* tar file doesn't contain root node */
	{
		Attr *a = new Attr;
		a->mode = 0775 | S_IFDIR;
		a->path = ROOT;

		attr[hasher(ROOT)] = a;
	}

	/* Connect FUSE API */
	memset(&op, 0, sizeof(op));
	op.getattr = my_getattr;
	op.readdir = my_readdir;
	op.read = my_read;

	return fuse_main(argc, argv, &op, NULL); 
}

int my_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	(void) offset; (void) fi;  // Unused

	if (path[1] == '\0') {  // Only root have trailing-slash
		for (auto name : dir[hasher(ROOT)])
			filler(buffer, name.c_str(), NULL, 0);
		return 0;
	}

	auto it = dir.find(hasher(path + ROOT));
	if (it == dir.end()) {
		printf("my_readdir: directory '%s' (%ld) not found.\n", path,  hasher(path + ROOT));
		return 1;
	}

	for (auto name : dir[hasher(path + ROOT)])
		filler(buffer, name.c_str(), NULL, 0);

	return 0;
}

int my_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi) {
	auto it = attr.find(hasher(path));
	if (it == attr.end())
		return -2;  // No such file
	Attr *a = attr[hasher(path)];

	if (offset < 0 || (size_t) offset >= a->size)
		return 0;

	size_t sz = size;
	if (sz > a->size - offset)
		sz = a->size - offset;

	tar.seekg(a->pos + offset);
	tar.read(buffer, sz);

	return sz;
}

int my_getattr(const char *path, struct stat *st) {
	auto it = attr.find(hasher(path));
	if (it == attr.end())
		return -2;  // No such file
	Attr *a = attr[hasher(path)];

	st->st_mode = a->mode;
	st->st_uid = a->uid;
	st->st_gid = a->gid;
	st->st_size = a->size;
	st->st_mtime = a->mtime;

	return 0;
}
