/*utf8encode - Encode binary files to valid UTF-8.
Copyright (C) 2014  nemasu@gmail.com

This file is part of utf8encode.

utf8encode is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

utf8encode is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with utf8encode.  If not, see <http://www.gnu.org/licenses/>.*/

#include <string>
#include <fstream>
#include <iostream>
#include <cstring>
#include <list>

using namespace std;

int count = 1; //1 based
int bitLoc = 0;//1 based
bool nextByte = true;
fstream stream;
char b = 0x00;
int fileLength = 0;
bool doneFile = false;
int pos = 1;

int getNextBit() {
	if( bitLoc == 0 ) {
		bitLoc = 8;
		b = stream.get();
		fileLength--;
		if( fileLength < 0 ) {
			doneFile = true;
			return 0;
		}
	}
	
	int ret = b & (1 << (bitLoc-1));
	ret = ret >> (bitLoc-1);
	ret &= 0x00000001;
	//printf("%.2X ", ret);
	bitLoc--;
	
	return ret;
}

int getNextEncodedBit() {
	
	if( bitLoc == 0 ) {
		
		b = stream.get();
		fileLength--;
		
		if( pos == 1 ) {
			b = (b & 0x0F);
			bitLoc = 4;
		} else {
			b = (b & 0x3F);
			bitLoc = 6;
		}
		
		pos++;
		
		if( pos == 4 ) {
			pos = 1;
		}
		if( fileLength < 0 ) {
			doneFile = true;
			return 0;
		}
	}
	
	int ret = b & (1 << (bitLoc-1));
	ret = ret >> (bitLoc-1);
	ret &= 0x00000001;
	//printf("%.2X ", ret);fflush(stdout);
	bitLoc--;
	
	return ret;

}

int main( int argv, char **argc ) {
	stream.open( argc[2], ios::binary | ios::in );
	if( !stream.is_open() ) {
		cerr << "Cannot open file: " << argc[2] << endl;
		return -1;
	}

	string flag(argc[1]);
	
	if( flag == "-d" ) {
		stream.seekg(0, stream.end);
        fileLength = stream.tellg();
        stream.seekg(0, stream.beg);
		list<char> decoded;
		char newByte = 0x00;
		char newByteCount = 8;
		int pos = 1;//1 based
		while(true) {
						
			newByteCount = 8;
			newByte = 0x00;

			while( newByteCount != 0 ) {
				int b = getNextEncodedBit();
				
				if( doneFile ) {
					break;
				}

				newByte |= (b << (newByteCount-1));

				newByteCount--;
				
			}
			
			if( doneFile ) {
				break;
			}

			decoded.push_back(newByte);
		}
	
		list<char>::iterator beg = decoded.begin();
		list<char>::iterator end = decoded.end();
		string outputFilename = string(argc[2]) + string(".decoded");
		fstream output( outputFilename.c_str(), ios::binary | ios::out );
		int lineBreak = 30;
		while( beg != end ) {
			printf( "%.2x ", *beg & 0xff );
			output.put(*beg);
			lineBreak--;
			if( lineBreak == 0 ) {
				lineBreak = 30;
				printf("\n");
			}
			beg++;
		}

		printf("\n");
	}
	
	if( flag == "-e" ) {

		stream.seekg(0, stream.end);
		fileLength = stream.tellg();
		stream.seekg(0, stream.beg);
		
		list<char> encoded;
		char b1 = 0xE0;
		char bn = 0x80;
		char newByte = 0;
		char codePointCount = 1; //1 based
		int newByteCount = 0;

		while( 1 ) {
			if( doneFile ) {
				break;
			}

			if( codePointCount == 1 ) {
				newByte = b1;
				newByteCount = 4;
			} else {
				newByte = bn;
				newByteCount = 6;
			}

			while( newByteCount != 0 ) {
				int b = getNextBit();
				
				if( doneFile ) {
					break;
				}
				
				//Put b at newByte:newByteCount
				newByte |= (b << (newByteCount-1));

				newByteCount--;
				
			}
			
			if( codePointCount == 3 ) {
				codePointCount = 1;
			} else {
				codePointCount++;
			}
			
			encoded.push_back(newByte);
		}

		list<char>::iterator beg = encoded.begin();
		list<char>::iterator end = encoded.end();
		int lineBreak = 30;
		string outputFilename = string(argc[2]) + string(".encoded");
		fstream output( outputFilename.c_str(), ios::binary | ios::out );
		while( beg != end ) {
			output.put(*beg);
			printf( "%.2x ", *beg & 0xff );
			lineBreak--;
			if( lineBreak == 0 ) {
				lineBreak = 30;
				printf("\n");
			}
			beg++;
		}

		printf("\n");
	}
	
	return 0;
}