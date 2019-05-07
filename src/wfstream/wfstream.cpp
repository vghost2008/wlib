#include <wfstream.h>
#include <QFile>
#include <iostream>

using namespace std;
void WSpace::wfstream::open(const char* filename,ios_base::openmode mode)
{
	open(string(filename),mode);
}
void WSpace::wfstream::open(const string& filename,ios_base::openmode mode)
{
	fstream::open(filename,mode);

	if(!is_open()) return;

	filename_ = filename;
	if(!header_.empty()) 
		(*this)<<header_<<endl;
}
WSpace::wfstream::~wfstream()
{
	if(is_open()) {
		flush();
		close();
	}
	if(delete_if_empty_ && empty()) {
		QFile::remove(filename_.c_str());
	}
}
uint64_t WSpace::wfstream::size()const 
{
	return size(filename_);
}
uint64_t WSpace::wfstream::size(const string& filename)
{
	return QFile(filename.c_str()).size();
}
bool WSpace::wfstream::empty()const 
{
	//外加一个换行符
	return size()<=(1+header_.size());
}
