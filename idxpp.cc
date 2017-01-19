#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#include <unicode/translit.h>
#include <unicode/unistr.h>

using namespace std;

extern const char *translit_cache_data[][2];
unordered_map<string, string> translit_cache;

bool find_delim(const string &s, const char delim[2], size_t &i, size_t &j)
{
	i = s.find_first_of(delim, i);
	if (i == string::npos || s[i] == delim[1])
		return false;
	int level = 1;
	j = i;
	while ((j = s.find_first_of(delim, j + 1)) != string::npos) {
		if (s[j] == delim[0]) ++level;
		if (s[j] == delim[1]) --level;
		if (level == 0) return true;
	}
	return false;
}

void translit(Transliterator *tr, string &s)
{
	auto it = translit_cache.find(s);

	if (it != translit_cache.end()) {
		s = it->second;
	} else {
		UnicodeString us = UnicodeString::fromUTF8(s);
		tr->transliterate(us);
		string &cache = translit_cache[s];
		s.clear();
		us.toUTF8String(s);
		cache = s;
	}
}

void transform(Transliterator *tr, string &s)
{
	size_t save = 0, pos = 0;
	int skip = 0;
	for (;;) {
		pos = s.find_first_of("@!|{", pos);
		if (pos == string::npos)
			break;
		char delim = s[pos];
		if (delim == '{') {
			find_delim(s, "{}", pos, pos);
			continue;
		}
		string sub = s.substr(save, pos-save);
		translit(tr, sub);
		if (delim == '@') {
			s.replace(save, pos, sub);
			pos = save + sub.length();
			skip = 1;
		} else if (!skip) { /* ! or | */
			sub.push_back('@');
			s.insert(save, sub);
			pos += sub.length();
		} else {
			skip = 0;
		}
		save = ++pos;
		if (delim == '|') {
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cerr << "missing input file\n";
		return 1;
	}
	fstream idxfile(argv[1], ios::in);
	if (idxfile.fail()) {
		cerr << "bad input file\n";
		return 1;
	}
	UErrorCode status = UErrorCode();
	Transliterator *tr = Transliterator::createInstance(L"Han-Latin; Latin-NumericPinyin", UTRANS_FORWARD, status);
	if (U_FAILURE(status)) {
		cerr << "bad transliterator: " << u_errorName(status) << '\n';
		return 1;
	}
	// init cache
	for (auto it = translit_cache_data; **it; it++) {
		translit_cache[(*it)[0]] = (*it)[1];
	}

	// translit
	for (string line; getline(idxfile, line); ) {
		size_t left = 0, right;
		if (!find_delim(line, "{}", left, right))
			continue;
		left++; right--;
		string substr = line.substr(left, right-left);
		transform(tr, substr);
		cout << line.substr(0, left) << substr << line.substr(right, line.length()) << '\n';
	}
	delete tr;
}
