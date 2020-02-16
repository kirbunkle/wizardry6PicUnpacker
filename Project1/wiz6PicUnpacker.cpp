#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

int compressDataAndWrite(vector<char>& dataIn, char* destFileName)
{
	int error = 0;
	int size = dataIn.size();
	vector<char> dataOut;
	int i = 0;
	int arrayLenPos = 0;
	char curLen = 0;
	int j = 0;

	while (i < size)
	{
		// put in a spot for the next byte array length
		if (j >= 0x1000) j = 0;
		arrayLenPos = dataOut.size();
		dataOut.push_back(0x00);
		j++; // j simulates writing out to a file every 0x1000 bytes, I would do that but meeehhhh
		curLen = 0;

		while ((i < size) && (curLen < 0x7F) && (j < 0x1000))
		{
			// look for a string of 3 or more bytes that match
			if ((i + 2 < size) && (dataIn[i] == dataIn[i + 1]) && (dataIn[i] == dataIn[i + 2]))
			{
				// at least the next 3 bytes match, let's keep going until we get a different byte
				if (curLen > 0)
				{
					break; // leave the inner loop so we can start a new array
				}
				char curByte = dataIn[i];
				dataOut.push_back(curByte); // we only add 1 and keep track of the count
				j++;
				curLen = 0x00;

				while ((i < size) && (curLen > -(0x80)) && (dataIn[i] == curByte))
				{
					curLen--;
					i++;
				}
				break;
			}
			else
			{
				// the next 3 bytes do not match, just get one at a time
				curLen++;
				dataOut.push_back(dataIn[i++]);
				j++;
			}
		}

		dataOut[arrayLenPos] = curLen;
	}

	// last thing to add is the 0x00 terminate byte
	dataOut.push_back(0x00);

	ofstream outFile(destFileName, ios::out | ios::binary);
	outFile.write((char*)&dataOut[0], dataOut.size());
	outFile.close();

	cout << "Saved compressed result to " << destFileName << endl;
	return error;
}

int readAndDecompressData(char* sourceFileName, vector<char>& dataOut)
{
	int error = 0;
	char buffer[0x1000];
	ifstream inFile(sourceFileName, ios::in | ios::binary);
	bool done = false;

	if (!inFile.good())
	{
		cout << "Error reading " << sourceFileName << endl;
		error = 2;
		done = true;
	}

	while (!done)
	{
		int i = 0;
		inFile.read(buffer, 0x1000); // original code reads in 0x1000 bytes at a time, guess we're doing the same thing
		if (inFile.bad())
		{
			cout << "Error reading " << sourceFileName << endl;
			error = 2;
			break;
		}
		while (i < 0x0FFF)
		{
			char byteIn = buffer[i++];
			if (byteIn == 0x00) 
			{
				// 0x00 is the signal to terminate
				done = true;
				break;
			}
			else if ((byteIn & 0x80) == 0x00)
			{
				// positive byte, read the next n bytes
				for (int j = byteIn; j > 0; j--)
				{
					dataOut.push_back(buffer[i++]);
				}
			}
			else
			{
				// negative byte, the next byte is repeated -n (bits flipped) times
				char byteOut = buffer[i++];
				for (int j = -byteIn; j > 0; j--)
				{
					dataOut.push_back(byteOut);
				}
			}
		}
		if ((inFile.eof() && !done) || (!inFile.eof() && done)) // could be xnor, but then how would you read it?
		{
			cout << "Invalid file format for " << sourceFileName << endl;
			error = 3;
			break;
		}
	}

	inFile.close();	

	return error;
}

int decompressPicToFile(char* sourceFileName, char* destFileName)
{
	vector<char> dataOut;
	int error = readAndDecompressData(sourceFileName, dataOut);
	if (error == 0)
	{
		ofstream outFile(destFileName, ios::out | ios::binary);
		outFile.write((char*)&dataOut[0], dataOut.size());
		outFile.close();

		cout << "Saved decompressed result to " << destFileName << endl;
	}
	return error;
}

int compressFileToPic(char* sourceFileName, char* destFileName)
{
	int error = 0;
	ifstream inFile(sourceFileName, ios::in | ios::binary | ios::ate);

	if (!inFile.good())
	{
		cout << "Error reading " << sourceFileName << endl;
		error = 2;
	}
	else
	{
		int size = (int)inFile.tellg();
		inFile.seekg(0, ios::beg);
		vector<char> dataIn(size);

		bool good = (bool)inFile.read(dataIn.data(), size);

		inFile.close();

		if (good)
		{
			error = compressDataAndWrite(dataIn, destFileName);
		}
		else
		{
			cout << "Error reading " << sourceFileName << endl;
			error = 2;
		}
	}
	return error;
}

int decompressPicToBmp(char* sourceFileName, char* destFileName)
{
	cout << "Warning: This function is incomplete, it doesn't save as a valid bitmap! You can still read and compress it back to pic though." << endl;
	vector<char> dataIn;
	int error = readAndDecompressData(sourceFileName, dataIn);
	if (error == 0)
	{
		vector<char> bmpData(dataIn.begin(), dataIn.end());
		
		// It appears that the header is the first 600 (0x258) bytes,
		// let's just write that out as-is so we don't lose track of it

		int i = 0x258;
		int size = dataIn.size();

		unsigned char readMask = 0x80;
		while (i < size)
		{
			int j = i;
			int jstart = j;
			while (j < jstart + 8)
			{
				char val = 0;

				if ((dataIn[j + 24] & readMask) != 0x00) val = val | 0x80;
				if ((dataIn[j + 16] & readMask) != 0x00) val = val | 0x40;
				if ((dataIn[j + 8] & readMask) != 0x00)  val = val | 0x20;
				if ((dataIn[j] & readMask) != 0x00)      val = val | 0x10;

				readMask = readMask >> 1;

				if ((dataIn[j + 24] & readMask) != 0x00) val = val | 0x08;
				if ((dataIn[j + 16] & readMask) != 0x00) val = val | 0x04;
				if ((dataIn[j + 8] & readMask) != 0x00)  val = val | 0x02;
				if ((dataIn[j] & readMask) != 0x00)      val = val | 0x01;

				readMask = readMask >> 1;

				if (readMask == 0x00)
				{
					j++;
					readMask = 0x80;
				}

				bmpData[i++] = val;
			}
		}
				
		ofstream outFile(destFileName, ios::out | ios::binary);

		/*
		// write header
		outFile
			<< "BM"
			<< (int)(bmpData.size() + 51) // file size
			<< "\0\0\0\0"
			<< (int)1078
			<< (int)40
			<< (int)4                     // image width
			<< (int)(bmpData.size() / 4)  // image height
			<< "\1\0"
			<< "\4\0"                     // bits per pixel (4)
			<< "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			;
		*/

		outFile.write((char*)&bmpData[0], bmpData.size());
		outFile.close();

		cout << "Saved decompressed result to " << destFileName << endl;
	}
	return error;
}

int compressBmpToPic(char* sourceFileName, char* destFileName)
{
	cout << "Warning: This function is incomplete, it doesn't read a valid bitmap! This will work with previously decompressed 'pseudo-BMP' files though." << endl;
	int error = 0;
	ifstream inFile(sourceFileName, ios::in | ios::binary | ios::ate);

	if (!inFile.good())
	{
		cout << "Error reading " << sourceFileName << endl;
		error = 2;
	}
	else
	{
		int size = (int)inFile.tellg();
		inFile.seekg(0, ios::beg);
		vector<char> dataIn(size);

		bool good = (bool)inFile.read(dataIn.data(), size);

		inFile.close();

		if (good)
		{
			vector<char> outData(dataIn.begin(), dataIn.end());

			// It appears that the header is the first 600 (0x258) bytes,
			// let's just write that out as-is so we don't lose track of it

			// let's zero out everything after the header, we are going to really be screwing with it
			for (int x = 0x258; x < (int)outData.size(); x++)
			{
				outData[x] = 0x00;
			}

			int i = 0x258;
			int size = dataIn.size();

			unsigned char readMask = 0x80;
			while (i < size)
			{
				int j = i;
				int jstart = j;
				while (j < jstart + 8)
				{
					char val = dataIn[i++];

					if ((val & 0x80) != 0x00) outData[j + 24] = outData[j + 24] | readMask;
					if ((val & 0x40) != 0x00) outData[j + 16] = outData[j + 16] | readMask;
					if ((val & 0x20) != 0x00) outData[j + 8]  = outData[j + 8]  | readMask;
					if ((val & 0x10) != 0x00) outData[j]      = outData[j]      | readMask;

					readMask = readMask >> 1;

					if ((val & 0x08) != 0x00) outData[j + 24] = outData[j + 24] | readMask;
					if ((val & 0x04) != 0x00) outData[j + 16] = outData[j + 16] | readMask;
					if ((val & 0x02) != 0x00) outData[j + 8]  = outData[j + 8]  | readMask;
					if ((val & 0x01) != 0x00) outData[j]      = outData[j]      | readMask;

					readMask = readMask >> 1;

					if (readMask == 0x00)
					{
						j++;
						readMask = 0x80;
					}
				}
			}

			error = compressDataAndWrite(outData, destFileName);
		}
		else
		{
			cout << "Error reading " << sourceFileName << endl;
			error = 2;
		}
	}
	return error;


}

void showHelp(char* programName)
{
	cout<< "Usage: " << programName << " [-cb,-db,-cr,-dr] [source file name] [dest file name]" << endl
		<< "  c : Compress unpacked file" << endl
		<< "  d : Decompress PIC file" << endl
		<< "  b : Unpacked file format is BMP (BETA! Currently not a valid BMP file, just a map of bit data organized like you would expect)" << endl
		<< "  r : Unpacked file format is raw data (this is how the data looks in memory of the game)" << endl
		<< endl
		<< "Examples:" << endl
		<< "  -dr: Read from compressed PIC file and write unpacked raw data to a binary file" << endl
		<< "  -cr: Read from unpacked raw binary file, compress and write to PIC file" << endl
		<< "  -db: Read from compressed PIC file, convert and write to pseudo-BMP file" << endl
		<< "  -cb: Read from previously decompressed pseudo-BMP file, compress and write to PIC file" << endl
	;
}

int main(int argc, char **argv)
{
	if ((argc == 4) && (argv[1][0] == '-') && (strlen(argv[1]) == 3))
	{
		if (argv[1][1] == 'c')
		{
			if (argv[1][2] == 'r')
				return compressFileToPic(argv[2], argv[3]);
			if (argv[1][2] == 'b')
				return compressBmpToPic(argv[2], argv[3]);
		}
		else if (argv[1][1] == 'd')
		{
			if (argv[1][2] == 'r')
				return decompressPicToFile(argv[2], argv[3]);
			if (argv[1][2] == 'b')
				return decompressPicToBmp(argv[2], argv[3]);
		}
	}
	
	showHelp(argv[0]);
	return 1;
}