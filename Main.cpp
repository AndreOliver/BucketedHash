// File Hash with fixed length records and Heap overflow
// Author - Andre Oliver
// COP 3540 - Introduction to Database Structures

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
using namespace std;

struct mdbStudent
	{	
		string sDelFlag;
		string sID;
		string sLastName;
		string sFirstName;
		string sGPA;
	};
// Function Headers
void Create();
void Dump();
void Insert();
void Delete();
void Print();
long CalcOverflow();
void ParseBlock(string line);
string intToString(int x);
mdbStudent ParseRecord(string psString);
int  * Find(string psSID);
void Retrieve();
int readInHeader;	    //number of records in this page
int readInOverflow;     //overflow flag
mdbStudent readInstudentInfo [3];
// Global Variables
const int MNRECORDLENGTH = 48;	// Length of each record 1(Delete Flag) + 3(Student_ID) + 20(Last Name) + 20(First Name) + 4 (GPA) + 2(CRLF)
const int MNPAGEWITHOVERFLOW = 156; //length of each hash bucket with overflow following
const int MNHASHSIZE = 10;		 // The number of hash records that can be held in the file
const string MCSFILENAME = "C:\\temp\\StudentGPAHash.dat";
fstream mioHashFile;			// The file we will be storing a records in
int mnOverFlowRecords = 0;			// number of records in overflow 



// Main program that presents a menu of options to the user and calls the various functions
int main()
{
	char sOption;
	cout << "Enter a Menu Selection - \n(C)reate, \n(I)nsert, \n(P)rint, \nX=Delete, \n(R)etrieve, \n(D)ump, \n(Q)uit" << endl;
	cin >> sOption;
	while (sOption != 'Q' && sOption != 'q')
	{
		switch ( sOption )
      {
         case 'C':
            Create();
            break;
		 case 'c':
            Create();
            break;
         case 'I':
            Insert();
            break;
         case 'i':
            Insert();
            break;
		 case 'P':
            Print();
            break;
         case 'p':
            Print();
            break;
		 case 'D':
            Dump();
            break;
         case 'd':
            Dump();
            break;
         case 'X':
            Delete();
            break;
         case 'x':
            Delete();
            break;
         case 'R':
            Retrieve();
            break;
		 case 'r':
            Retrieve();
            break;
         default:
			 cout << "Invalid Menu Option Entered \n";
		}

	cout << "Enter a Menu Selection - \n(C)reate, \n(I)nsert, \n(P)rint, \nX=Delete, \n(R)etrieve, \n(D)ump, \n(Q)uit" << endl;
		cin >> sOption;

	}
	return 0;
}
// This function opens the emp file for output which will remove all records already out there
// It then writes a header record that contains the overflow record number (0)
// and then writes 10 empty records into the file
string intToString(int x)
{
	string result;
	char temp[256];
	//itoa(x,temp,10);
	//_itoa(x,temp,10);
	_itoa_s(x,temp,255,10);
	result=temp;
	return result;
}
void Create()
{
	
	mioHashFile.open(MCSFILENAME,ios::out | ios::trunc);
	
	// Initialize student file with fixed length dummy records. 
	// Assume there are 10 records in the file.	
 
	for (int nRecords = 0; nRecords <= 10; nRecords++)
	{
		mioHashFile << "    0   -10                                               0                                               0                                               " << endl;
		mioHashFile.seekp(nRecords*MNPAGEWITHOVERFLOW);
	}
	mioHashFile.close();
	cout << "create complete\n";
	return;
}
// This function is used to insert a new record into out student file.
// It first calculates the hash for the record and reads that record to determine
// if it has been used before.  If it has not, it writes the new record and returns.
// If the primary data record has been used, it then checks to see if either the
// primary record or some record is overflow matches the key to the new record, and
// if a match is found, it reports this and returns.
// While searching for a match, the program records the first deleted record so that it 
// might be used again.
// If no match is found, and a deleted record was found, the new record is written over
// the deleted record and the function returns.  If no deleted record was found
// the overflow pointer is accessed, the new record written to overflow, and the overflow
// pointer incremented.
void Insert()
{
	int ioDeleteRecordPointer = -1; // Pointer to the address of the first deleted record
	// Data accepted from CIN 
	mdbStudent ioInput;

	// Data from the record being examined
	mdbStudent ioCurrentRecord;

	int nID;			/* integer value of primary key string sInID */
	int nHashKey;		/* hash value = (key % 10) */
	long nFileOffset;	/* offset */
	string sRecord;


	mioHashFile.open(MCSFILENAME,ios::in | ios::out);
	//mnOverFlowRecords = CalcOverflow();			// Get the number of overflow records in the file.  Needed when we want to add another
	//Get the overflow header from the file

	cout << "Please enter Student_ID, Last-Name, First-Name and GPA separated by blanks\n";
	cin >> ioInput.sID >> ioInput.sLastName >> ioInput.sFirstName >> ioInput.sGPA;
	

	// convert primary key string to integer by folding and then division
	nID = stoi(ioInput.sID);
	nHashKey = nID  % MNHASHSIZE;

	// calculate the address in the file for the primary record, set the file pointer
	// there and read the record
	nFileOffset = (nHashKey * MNPAGEWITHOVERFLOW);			//reads in the whole lane that the id hashes to 
	mioHashFile.seekg(nFileOffset);							
	getline(mioHashFile,sRecord);
	ParseBlock(sRecord);										//parses the line to the static values
	int nFileOffsetIncremented = nFileOffset;					//this is used to increment through the data by pages
	
	/*checks the primary record, if its never been used then insert immediately*/
	if (readInstudentInfo[0].sDelFlag.compare("0")==0)  // virgin record, insert immediately 
	{
		mioHashFile.seekp(nFileOffset); // Back up to the beginning if the record
		//cout<<"deleteflag in overflow: "<<readInstudentInfo[1].sDelFlag;
		mioHashFile <<"    1"<<"   -1"<< "1" << setw(3) << ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<<readInstudentInfo[1].sDelFlag<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA<<readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
		if (mioHashFile.bad())
		{
			cout << "Crap" << endl;
		}

		mioHashFile.close();
		cout << "Insert Complete" << endl;
		return;
	}

	// A live record with the same SSN - Report it and get out
	if (readInstudentInfo[0].sDelFlag.compare("1")==0 && readInstudentInfo[0].sID.compare(ioInput.sID)==0)
	{
		cout << "this student id already exists!"  << endl;
		mioHashFile.close();
		return;
	}

	// Found deleted record.  overwrite the deleted record in primary area /*this*/
	if (readInstudentInfo[0].sDelFlag.compare("2")==0 )  
	{
		mioHashFile.seekp(nFileOffset);
		mioHashFile <<setw(5)<<intToString(readInHeader)<<setw(5)<<intToString(readInOverflow)<<"1"<< ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<<readInstudentInfo[1].sDelFlag<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA<<readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA;
		mioHashFile.close();
		return;
	}

	// Search through overflow to see if the record exists there
	/* a do-while loop is used to go to the hashed page, check all records and then use the overflow pointer to go to the next
	page in the and so on... until a negative value is reached*/
	nFileOffsetIncremented = nFileOffset;
	do
	{
		mioHashFile.seekg(nFileOffsetIncremented);
		getline(mioHashFile,sRecord);
		ParseBlock(sRecord);
		// Duplicate primary key in overflow area.  Abort insert. 
		for(int j = 0; j<readInHeader; j++)
		{
			if (readInstudentInfo[j].sDelFlag.compare("1")==0 && readInstudentInfo[j].sID.compare(ioInput.sID)==0)
			{
				cout << "this student id already exists!"  << endl;
				mioHashFile.close();
				return;
			}
		}

		nFileOffsetIncremented = (readInOverflow-1) * MNPAGEWITHOVERFLOW;
		
	}while(nFileOffsetIncremented > 0);


	
//insert to overflow - this will read through ALL existing overflow for a suitable place for insertion	
	nFileOffsetIncremented = nFileOffset; //resets nFileOffsetIncremented to the first hashed page
	do
	{
		nFileOffset = nFileOffsetIncremented;  //saves the second to last position "used to link new pages to old ones"
		mioHashFile.seekg(nFileOffsetIncremented);
		getline(mioHashFile,sRecord);
		ParseBlock(sRecord);
		
			
			if (readInstudentInfo[0].sDelFlag.compare("2")==0)
			{
				mioHashFile.seekp(nFileOffsetIncremented);
				mioHashFile <<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<<"1"<<ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<<readInstudentInfo[1].sDelFlag<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA <<readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
				mioHashFile.close();
				return;
			}
			//if theres a virgin key available and no deleted keys then write to virgin entry
			if (readInstudentInfo[0].sDelFlag.compare("0")==0)
			{
				mioHashFile.seekp(nFileOffsetIncremented);
				readInHeader+=1;
				mioHashFile <<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<<"1"<<ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<<readInstudentInfo[0].sDelFlag<<readInstudentInfo[0].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA <<readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
				mioHashFile.close();
				return;
			}
			
			if (readInstudentInfo[1].sDelFlag.compare("2")==0)
			{
				mioHashFile.seekp(nFileOffsetIncremented);
				mioHashFile <<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<< readInstudentInfo[0].sDelFlag<<readInstudentInfo[0].sID<<readInstudentInfo[0].sLastName<<readInstudentInfo[0].sFirstName<<readInstudentInfo[0].sGPA <<"1"<<ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<<readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
				mioHashFile.close();
				return;
			}
			//if theres a virgin key available and no deleted keys then write to virgin entry
			if (readInstudentInfo[1].sDelFlag.compare("0")==0)
			{
				mioHashFile.seekp(nFileOffsetIncremented);
				readInHeader+=1;
				mioHashFile <<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<< readInstudentInfo[0].sDelFlag<<readInstudentInfo[0].sID<<readInstudentInfo[0].sLastName<<readInstudentInfo[0].sFirstName<<readInstudentInfo[0].sGPA <<"1"<<ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<<readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
				mioHashFile.close();
				return;
			}

			if (readInstudentInfo[2].sDelFlag.compare("2")==0)
			{
				mioHashFile.seekp(nFileOffsetIncremented);
				mioHashFile <<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<< readInstudentInfo[0].sDelFlag<<readInstudentInfo[0].sID<<readInstudentInfo[0].sLastName<<readInstudentInfo[0].sFirstName<<readInstudentInfo[0].sGPA <<readInstudentInfo[1].sDelFlag<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA<<"1"<<ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<<endl;
				mioHashFile.close();
				return;
			}
			//if theres a virgin key available and no deleted keys then write to virgin entry
			if (readInstudentInfo[2].sDelFlag.compare("0")==0)
			{
				mioHashFile.seekp(nFileOffsetIncremented);
				readInHeader+=1;
				mioHashFile <<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<< readInstudentInfo[0].sDelFlag<<readInstudentInfo[0].sID<<readInstudentInfo[0].sLastName<<readInstudentInfo[0].sFirstName<<readInstudentInfo[0].sGPA <<readInstudentInfo[1].sDelFlag<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA<<"1"<<ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<<endl;
				mioHashFile.close();
				return;
			}
			
				nFileOffsetIncremented = (readInOverflow-1)*MNPAGEWITHOVERFLOW; //send to next overflow page
			
		
		
	}while(nFileOffsetIncremented > 0); 
		

		
		//create overflow and insert to virgin record
		//if all overflow record spaces are full then check for overflow, if none exist then create a new block
		//if overflow is full... make another overflow page

		//if the info gets to this point, then there are no deleted records and the overflow is full so we make another block for overflow. and sset the previous overflow pointer to this location
	if( readInstudentInfo[0].sDelFlag.compare("1")==0 && readInstudentInfo[1].sDelFlag.compare("1")==0 && readInstudentInfo[2].sDelFlag.compare("1")==0) 
		{
			//if all records in primary and overflows are full then create overflow and add there
			mioHashFile.clear();
			if(mioHashFile.is_open())
			{
				mioHashFile.close();
			}
			//add row with one entry to the end of file
			mioHashFile.open(MCSFILENAME,ios::out | ios::in | ios::app);
			mioHashFile <<"    1"<<"   -1"<< "1" << setw(3) << ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA<< "0                                               0                                               " << endl;
			mioHashFile.close();
			//overwrite previous row to reflect the new overflow location
			mioHashFile.open(MCSFILENAME,ios::out | ios::in);
			int temp = CalcOverflow();
			mioHashFile.seekp(nFileOffset);
			mioHashFile <<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(temp)<< readInstudentInfo[0].sDelFlag<<readInstudentInfo[0].sID<<readInstudentInfo[0].sLastName<<readInstudentInfo[0].sFirstName<<readInstudentInfo[0].sGPA <<readInstudentInfo[1].sDelFlag<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA<< readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
			mioHashFile.close();
			return;
		}
	// We are now here, so we have searched through primary and overflow and
	// did not find a duplicate SSN
	// See if we found a deleted record we can reuse
	if (ioDeleteRecordPointer != -1 )
	{
		mioHashFile.seekp(ioDeleteRecordPointer); // Back up to the beginning if the record
		mioHashFile << "1" << setw(3) << ioInput.sID << left << setw(20) << ioInput.sLastName << setw(20) << ioInput.sFirstName << setw(4) << right << ioInput.sGPA << endl;

	}
	
	// All done - Say Goodbye!!
	mioHashFile.close();
	cout<<"insert complete"<<endl;
	return;

}
// This function simply uses the DOS Type command to dump the contents of the file
void Dump()
{ 
	string sCommand = "type " + MCSFILENAME;
	system(sCommand.c_str());
	return;
}

// The Find Function is passed a student ID 
// It will look through the primary and overflow records for a 
// the same key and if found pass back the relative position to the beginning
// of that record.  If nothing is found, a -1 is passed back.
long  find(string psSID)  /* key is pointer to SSN */
{
	int   nSSN, nHashKey;
	long nFileOffset = -1;
	mdbStudent ioCurrentRecord ;
	string sRecord;

	nSSN = stoi(psSID);
	nHashKey = nSSN % 10;  /* calculate hash value */
	nFileOffset = nHashKey * (MNPAGEWITHOVERFLOW) ;
	int nFileOffsetIncremented = nFileOffset;

	do
	{
		mioHashFile.seekg(nFileOffsetIncremented);
		getline(mioHashFile,sRecord);
		ParseBlock(sRecord);

		for (int nRecords = 0; nRecords <= readInHeader; nRecords++)
		{
			if (readInstudentInfo[0].sDelFlag.compare("1")==0 && readInstudentInfo[0].sID.compare(psSID)==0)
			{
				cout<<"found it"<<endl;
				mioHashFile.close();
				return nFileOffsetIncremented;
			}			
			if (readInstudentInfo[1].sDelFlag.compare("1")==0 && readInstudentInfo[1].sID.compare(psSID)==0)
			{
				cout<<"found it"<<endl;
				mioHashFile.close();
				return nFileOffsetIncremented;
			}
		
			if (readInstudentInfo[2].sDelFlag.compare("1")==0 && readInstudentInfo[2].sID.compare(psSID)==0)
			{
				cout<<"found it"<<endl;
				mioHashFile.close();
				return nFileOffsetIncremented;
			}
		} 
			nFileOffsetIncremented = (readInOverflow-1)*MNPAGEWITHOVERFLOW;
		
	}while(nFileOffsetIncremented > 0);
	//if we get this far and no match then return -1 for a no match situation.
	mioHashFile.close();
	return nFileOffsetIncremented = -1;
}

string padTo(string str, const size_t num)
{
	//adds spaces to the right of a string
	while(num > str.size())
		{
			str += " ";
		}
	str=str.substr(0,num);
	return str;
}
// The Retreive Function uses the Find Command to set the file pointer
// at the appropriate record.  It then reads the record and prints out
// the data in the record.
void Retrieve()
{
	string sInTarget;
	string sInName;
	mdbStudent ioCurrentRecord ;
	string sRecord;
	string line;
	long nFileOffset;
	int nNumberRecords=0;

	cout << "Please enter a Field name then a Field value" << endl;
	cin >> sInName >> sInTarget;

	mioHashFile.open(MCSFILENAME,ios::in | ios::out);
	mnOverFlowRecords = CalcOverflow();			// Get the number of overflow records in the file.  Needed when we want to add another

	if(sInName.compare("id")==0)
	{
		nFileOffset = find(sInTarget);	
		mioHashFile.open(MCSFILENAME,ios::in | ios::out);			// gotta reopen the file because find closes it	

		if (nFileOffset == -1)
		{
			printf("Student not found\n");
			mioHashFile.close();
			return;
		}
	
		mioHashFile.seekp (nFileOffset);  

		mioHashFile.seekg(nFileOffset);
		getline(mioHashFile,sRecord);
		ParseBlock(sRecord);

		for(int j = 0; j<readInHeader; j++)
		{

			if (readInstudentInfo[j].sID.compare(sInTarget)==0)
			{
				cout << "Student ID = " << readInstudentInfo[j].sID << " Name = " << readInstudentInfo[j].sFirstName << " " << readInstudentInfo[j].sLastName << " GPA = " << readInstudentInfo[j].sGPA << endl;
				nNumberRecords +=1;
				
			}
		}
		cout << "Print Complete...Number of Records = " << nNumberRecords <<  endl << endl;
		mioHashFile.close();
		return;
	}

	if(sInName.compare("last")==0)
	{
		sInTarget = padTo(sInTarget,20);
		/*sequentially search through the whole data structure for a match
		the structure could be any number pf pages tall so the height is
		calculated before searching*/
		for (int i = 0; i<CalcOverflow(); i++)
		{
			mioHashFile.seekg(i*MNPAGEWITHOVERFLOW);
			getline(mioHashFile,line);
			ParseBlock(line);

			for(int j = 0; j<readInHeader; j++)
			{
				if (readInstudentInfo[j].sLastName.compare(sInTarget)==0)
				{
					cout << "Student ID = " << readInstudentInfo[j].sID << " Name = " << readInstudentInfo[j].sFirstName << " " << readInstudentInfo[j].sLastName << " GPA = " << readInstudentInfo[j].sGPA << endl;
					nNumberRecords = nNumberRecords + 1;
				}
			}
		}
		cout << "Print Complete...Number of Records = " << nNumberRecords <<  endl << endl;
		mioHashFile.close();
		return;
	}

	if(sInName.compare("first")==0)
	{
		sInTarget = padTo(sInTarget,20);
		/*sequentially search through the whole data structure for a match
		the structure could be any number pf pages tall so the height is
		calculated before searching*/
		for (int i = 0; i<CalcOverflow(); i++)
		{
		
			mioHashFile.seekg(i*MNPAGEWITHOVERFLOW);
			getline(mioHashFile,line);
			ParseBlock(line);

			for(int j = 0; j<readInHeader; j++)
			{

				if (readInstudentInfo[j].sFirstName.compare(sInTarget)==0)
				{
					cout << "Student ID = " << readInstudentInfo[j].sID << " Name = " << readInstudentInfo[j].sFirstName << " " << readInstudentInfo[j].sLastName << " GPA = " << readInstudentInfo[j].sGPA << endl;
					nNumberRecords = nNumberRecords + 1;
				}
			}
		}
		cout << "Print Complete...Number of Records = " << nNumberRecords <<  endl << endl;
		mioHashFile.close();
		return;
	}
	if(sInName.compare("gpa")==0)
	{
		sInTarget = padTo(sInTarget,4);
		/*sequentially search through the whole data structure for a match
		the structure could be any number pf pages tall so the height is
		calculated before searching*/
		for (int i = 0; i<CalcOverflow(); i++)
		{
		
			mioHashFile.seekg(i*MNPAGEWITHOVERFLOW);
			getline(mioHashFile,line);
			ParseBlock(line);

			for(int j = 0; j<readInHeader; j++)
			{

				if (readInstudentInfo[j].sGPA.compare(sInTarget)==0)
				{
					cout << "Student ID = " << readInstudentInfo[j].sID << " Name = " << readInstudentInfo[j].sFirstName << " " << readInstudentInfo[j].sLastName << " GPA = " << readInstudentInfo[j].sGPA << endl;
					nNumberRecords = nNumberRecords + 1;
				}
			}
		}
		cout << "Print Complete...Number of Records = " << nNumberRecords <<  endl << endl;
		mioHashFile.close();
		return;
	}
	mioHashFile.close();
	return;

}

// The Delete function calls the Find Function.  If Find returns
// an active file pointer, then the routine updates the 
// the delete flag for location returned.
void Delete()
{
	string sInID;
	mdbStudent ioCurrentRecord ;
	string sRecord;
	long nFileOffset;

	cout << "Please enter Student ID" << endl;
	cin >> sInID;

	mioHashFile.open(MCSFILENAME,ios::in | ios::out);
	mnOverFlowRecords = CalcOverflow();			// Get the number of overflow records in the file.  Needed when we want to add another

	nFileOffset = find(sInID);	
	mioHashFile.open(MCSFILENAME,ios::in | ios::out);			// gotta reopen the file because find closes it	

	if (nFileOffset == -1)
	{
		printf("Student not found\n");
		mioHashFile.close();
		return;
	}
	
	mioHashFile.seekp (nFileOffset);  

	if (readInstudentInfo[0].sDelFlag.compare("1")==0 && readInstudentInfo[0].sID.compare(sInID)==0)
	{
		mioHashFile<<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<<"2"<<readInstudentInfo[0].sID<<readInstudentInfo[0].sLastName<<readInstudentInfo[0].sFirstName<<readInstudentInfo[0].sGPA<<readInstudentInfo[1].sDelFlag<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA<<readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
		mioHashFile.close();
		cout << "delete complete" << endl;
		return;
	}			
	if (readInstudentInfo[1].sDelFlag.compare("1")==0 && readInstudentInfo[1].sID.compare(sInID)==0)
	{
		mioHashFile<<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<<readInstudentInfo[0].sDelFlag<<readInstudentInfo[0].sID<<readInstudentInfo[0].sLastName<<readInstudentInfo[0].sFirstName<<readInstudentInfo[0].sGPA<<"2"<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA<<readInstudentInfo[2].sDelFlag<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
		mioHashFile.close();
		cout << "delete complete" << endl;
		return;
	}
		
	if (readInstudentInfo[2].sDelFlag.compare("1")==0 && readInstudentInfo[2].sID.compare(sInID)==0)
	{
		mioHashFile<<right<<setw(5)<<intToString(readInHeader)<<right<<setw(5)<<intToString(readInOverflow)<<readInstudentInfo[0].sDelFlag<<readInstudentInfo[0].sID<<readInstudentInfo[0].sLastName<<readInstudentInfo[0].sFirstName<<readInstudentInfo[0].sGPA<<readInstudentInfo[1].sDelFlag<<readInstudentInfo[1].sID<<readInstudentInfo[1].sLastName<<readInstudentInfo[1].sFirstName<<readInstudentInfo[1].sGPA<<"2"<<readInstudentInfo[2].sID<<readInstudentInfo[2].sLastName<<readInstudentInfo[2].sFirstName<<readInstudentInfo[2].sGPA<<endl;
		mioHashFile.close();
		cout << "delete complete" << endl;
		return;
	}
	


	mioHashFile.close();
	cout << "something went terribly wrong :(" << endl;
	return;
}

//This function prints out all the records in the employee file including 
//primary and overflow records.
void Print()
{
	mdbStudent ioCurrentRecord;
	string line;

	mioHashFile.open(MCSFILENAME,ios::in);
	int nNumberRecords = 0;

	mioHashFile.clear();
	
	/* sequentially print all active records */
	//this for loop has to stay because the datastructure dynamically grows, calcoverflow() returns the number of rows
	//which allows me to sequentially move through and print good records
	for (int i = 0; i<CalcOverflow(); i++)
	{
		
		mioHashFile.seekg(i*MNPAGEWITHOVERFLOW);
		getline(mioHashFile,line);
		ParseBlock(line);

		for(int j = 0; j<readInHeader; j++)
		{

			if (readInstudentInfo[j].sDelFlag.compare("1")==0)
			{
				cout << "Student ID = " << readInstudentInfo[j].sID << " Name = " << readInstudentInfo[j].sFirstName << " " << readInstudentInfo[j].sLastName << " GPA = " << readInstudentInfo[j].sGPA << endl;
				nNumberRecords = nNumberRecords + 1;
			}
		}
		
	}

	mioHashFile.close();
	cout << "Print Complete...Number of Records = " << nNumberRecords <<  endl << endl;
	return;

}

// This function gets the beginning and end of the file to calculate the number of overflow records
long CalcOverflow()
{
	streampos nBegin,nEnd;
	mioHashFile.seekg (0, ios::beg);
	nBegin = mioHashFile.tellg();
	mioHashFile.seekg (0, ios::end);
	nEnd = mioHashFile.tellg();

	return (((nEnd - nBegin) / MNPAGEWITHOVERFLOW));
}

mdbStudent ParseRecord(string psRecord) // Function that parses a record and returns the fields in a structure
{
	mdbStudent dbHoldRecord;
	dbHoldRecord.sDelFlag = psRecord.substr(0,1);
	dbHoldRecord.sID = psRecord.substr(1,3);
	dbHoldRecord.sLastName = psRecord.substr(4,20);
	dbHoldRecord.sFirstName = psRecord.substr(24,20);
	dbHoldRecord.sGPA = psRecord.substr(44,4);
	return dbHoldRecord;
}

//should have called this ParsePage because thats what it does lol
void ParseBlock( string line )
{
	readInHeader = stoi(line.substr(0,5));
	readInOverflow = stoi(line.substr(5,5));
	readInstudentInfo[0] = ParseRecord(line.substr(10,48));
	readInstudentInfo[1] = ParseRecord(line.substr(58,48));
	readInstudentInfo[2] = ParseRecord(line.substr(106,48));
}

