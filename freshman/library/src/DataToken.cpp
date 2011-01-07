#include "DataToken.h"
#include <vector>
#include <memory.h>
#include <fcntl.h>
#include <assert.h>
#include <set>
#include <string>
#include <map>
#include <cstdlib>
#include <iostream>
#include <algorithm>
using namespace std;

DataToken::DataToken() {
	active = false;
}

DataToken::DataToken(const wstring &filename, DT_MODE mode) {
	active = false;
	assert(open(filename, mode));
}

DataToken::~DataToken() {
	if (active) assert(close());
}

bool DataToken::open(const wstring &filename, DT_MODE mode) {
	if (active && !close()) return false;
	O_MODE = mode;
	string str(filename.begin(), filename.end());
	int openflags = O_WRONLY;
	if (mode == O_READ) openflags = O_RDONLY;
	fd = ::open(str.c_str(), openflags);
	if (fd == -1) return false;
	active = true;
	return true;
}

bool DataToken::write(const void *dat, size_t size) {
	if (!active || O_MODE != O_WRITE) return false;
	if (::write(fd, (void *)&size, sizeof(size_t)) == -1) return false;
	if (::write(fd, dat, size) == -1) return false;
	return true;
}

void *DataToken::read() {
	if (!active || O_MODE != O_READ) return false;
	size_t size;
	if (::read(fd, (void *)&size, sizeof(size_t)) == 0) return 0;
	void *dat = malloc(size + 10);
	if (!dat) return 0;
	memset(dat, 0, size + 10);
	if (::read(fd, dat, size) == 0) {
		free(dat);
		dat = 0;
		return 0;
	}
	return dat;
}

bool DataToken::close() {
	if (!active) return false;
	if (::close(fd) == -1) return false;
	active = false;
	return true;
}

int compare(const wstring &a, const wstring &b) {
	int result = 0;
	for (int i = 0; i < (int)a.length(); ++ i)
		for (int j = 0; j < (int)b.length(); ++ j) {
			int k = 0;
			while (a[i + k] == b[j + k]) ++ k;
			result = max(result, k);
		}
	return result;
}

wstring trim(wstring str) {
	wstring::iterator it = str.begin();
	while (str.begin() != str.end() && *str.begin() == L' ')
		str.erase(str.begin());
	while (str.begin() != str.end()) {
		it = str.end() - 1;
		if (*it != L' ') break;
		str.erase(it);
	}
	return str;
}

vector <wstring> split(wstring str) {
	vector <wstring> ans;
	ans.clear();
	wstring tmp = L"", strtmp;
	for (int i = 0; i < (int)str.length(); ++ i) {
		if (str[i] == L'，' || str[i] == L',') {
			strtmp = trim(tmp);
			if (strtmp.length() != 0u)
				ans.push_back(strtmp);
			tmp = L"";
		} else
			tmp += str[i];
	}
	strtmp = trim(tmp);
	if (strtmp.length() != 0u)
		ans.push_back(strtmp);
	return ans;
}

