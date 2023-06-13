//ALPER KUTAY OZBEK 2375467
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LENGTH_OF_FAT 4096
#define LENGTH_OF_FILE_LIST 128
#define LENGTH_OF_DATA 512
#define LENGTH_OF_DATA_LIST 4096
char file_name[50] = "disk.image";
FILE *my_disk ;//my disk is defined 
//structrues are defined
typedef struct{
	unsigned int table[LENGTH_OF_FAT];
}FAT;
typedef struct{
	char fileName[248];
	unsigned int firstBlock;
	long fileSize;
}File_List_Item;

typedef struct{
	File_List_Item items[LENGTH_OF_FILE_LIST];
}fileList;

typedef struct{
	char chunk[LENGTH_OF_DATA];
}data_chunk;

typedef struct{
	data_chunk data[LENGTH_OF_DATA_LIST];
}Total_Data;

typedef struct 
{
	FAT fat;
	fileList File_list;
	Total_Data TotData;
}disk;//IT is special because we are going to use allmost every function


unsigned int bigToLittleEndian(unsigned int value) {//big to little endian converter
    return ((value >> 24)& 0xff)  | ((value  >> 8)& 0xff00) | ((value  << 8)& 0xff0000) | ((value << 24)& 0xff000000) ;
}

long size_of_file(FILE *source);
unsigned int nearest_empty_index(FAT fat);
int find_file_disk(disk MYDSK,char* filename );
void print_file_list(fileList File_list);


void print_file_list_to_the_file();
void print_fat_to_the_file();
void List();
void Format();
void write(char* source_file, char* destination_file);
void delete(char* source_file);
void defragmentation();

unsigned int nearest_empty_index(FAT fat){//it finds nearest fat table index
	for (int i = 0; i < LENGTH_OF_FAT; i++)
	{
		if (fat.table[i]==0x00)
		{
			return i;
		}
	}
}
unsigned int nearest_empty_list(fileList File_List){//it finds nearest file list index
	for (int i = 0; i < LENGTH_OF_FILE_LIST; i++)
	{
		if (File_List.items[i].fileSize == 0x00)
		{
			return i;
		}
	}
}
long size_of_file(FILE *source){//It calculates file size as kB
	fseek(source, 0, SEEK_END);//before reading file adjust the pointer to the beggining of the file
	long fileSize = ftell(source);
	fseek(source, 0, SEEK_SET);
	return fileSize;
}

void List(){
	fileList File_list ;// define temp fileList
	disk MYDSK;//define disk 
	fread(&MYDSK, sizeof(MYDSK), 1, my_disk);//copy disk.image to disk structure
	print_file_list(MYDSK.File_list);//print the filelist of the MYDSK which is directly copy from disk.image
}

void print_space(int num_of_space)// we are using it in print state function
{//This funciton is used for blank adjustments
    for (int i = 0; i < num_of_space; i++)
    {
        printf(" ");
    }
}

void print_file_list(fileList File_list){
	int count=0;
	int name_lenght;
	printf("file name");
	print_space(30-strlen("file name"));//space prints 
	printf("file size\n");
	for (int i = 0; i < LENGTH_OF_FILE_LIST; i ++) {
		if(File_list.items[i].fileSize != 0)//if filesize is 0, it means it is not a file
		{
			count++;//count files
			printf("%s",File_list.items[i].fileName);
			name_lenght = strlen(File_list.items[i].fileName);
			print_space(30-name_lenght);//print spaces according to file name
			printf("%ld\n", File_list.items[i].fileSize);
		}
    }
	if (count == 0)
	{
		printf("Disk is empty...\n");//if there is no file print this
	}
}
int find_file_disk(disk MYDSK,char* filename ){//finds index of file in the disk.image
	for (int i = 0; i < LENGTH_OF_FILE_LIST; i++)
	{
		if (!strcmp(MYDSK.File_list.items[i].fileName,filename))
		{
			return i;
		}
	}
	return -1;// if it cannot be found in disk.image then return -1
}

void Format(){
	disk MYDSK;//define disk
	unsigned int HEADER = 0xFFFFFFFF;
	unsigned int OTHERS = 0x00000000;
	FAT fat;
	fileList File_list;
	Total_Data Tdata;
	memset(&MYDSK.File_list,0,sizeof(fileList));//define all structures to the zero
	memset(&MYDSK.fat,0,sizeof(FAT));
	memset(&MYDSK.TotData,0,sizeof(Total_Data));
	MYDSK.fat.table[0] = HEADER;//only the first member is changed to Header variable 
	fseek(my_disk, 0, SEEK_SET);//get pointer to the beggining to the file
	fwrite(&MYDSK, sizeof(MYDSK), 1, my_disk);//write temp disk to the actual disk
	fseek(my_disk, 0, SEEK_SET);
	//printf("Disk has been formatted.\n");
}

void write(char* source_file, char* destination_file){
	FILE *source = fopen(source_file,"rb+");//open the source file
	disk MYDSK;//define temp disk

	unsigned int chunk_count,first_fat_index,file_list_index,previous_index=0,current_index=0;

	fread(&MYDSK, sizeof(MYDSK), 1, my_disk);//copy content of my_disk
	long file_size = size_of_file(source);//calculate size of the source
	chunk_count = (file_size/sizeof(data_chunk))+1;//calculate number of data_chunk to be needed
	unsigned char buffer[chunk_count][LENGTH_OF_DATA];//according to number of chunks crate buffer to get data from source

	fread(buffer,file_size,1,source);// read data from source to buffer
	for (int i = 0; i < chunk_count; i++)
	{
		if (i == 0)//file information should be placed to the File_list
		{
			first_fat_index = nearest_empty_index(MYDSK.fat);
			file_list_index = nearest_empty_list(MYDSK.File_list);
			previous_index = first_fat_index;

			memcpy(MYDSK.File_list.items[file_list_index].fileName,destination_file,strlen(destination_file));//copy file name
			MYDSK.File_list.items[file_list_index].fileSize = file_size;//copy file info
			MYDSK.File_list.items[file_list_index].firstBlock = first_fat_index;//copy file info
			MYDSK.fat.table[previous_index] = 0x12345678;//trivial data to make this chunk is filled becuase if it was not filled, we would not find new empty index
			memcpy(&MYDSK.TotData.data[first_fat_index],&buffer[i],sizeof(data_chunk));//copy first chunk of data
		}
		else if (i == (chunk_count-1))//Make Last Fat table place as 
		{
			current_index = nearest_empty_index(MYDSK.fat);//there are two nearest_empty_index function becuase, we filled current and previous indexes in this condition
			MYDSK.fat.table[previous_index] = bigToLittleEndian(current_index);
			current_index = nearest_empty_index(MYDSK.fat);
			MYDSK.fat.table[current_index] = 0xFFFFFFFF;//make last index like this to show it is done
			memcpy(&MYDSK.TotData.data[current_index],&buffer[i],sizeof(data_chunk));
		}
		else if ((i != chunk_count-1) && (i>0))//Make Last Fat table place as 
		{
			current_index = nearest_empty_index(MYDSK.fat);//find the current fat index
			MYDSK.fat.table[previous_index] = bigToLittleEndian(current_index);//filled previous index with current index
			MYDSK.fat.table[current_index] = 0x12345678;//trivial data to make this chunk is filled
			memcpy(&MYDSK.TotData.data[current_index],&buffer[i],sizeof(data_chunk));//copy data content of buffer to the temp disk
			previous_index = current_index;
		}		
	}
	fseek(my_disk, 0, SEEK_SET);//get pointer to the end	
	fwrite(&MYDSK, sizeof(MYDSK), 1, my_disk);//and then writes
	fseek(my_disk, 0, SEEK_SET);
	
}
void read(char* source_file, char* destination_file){
	disk MYDSK;

	unsigned int file_size, fat_index, list_index;
	
	fread(&MYDSK, sizeof(MYDSK), 1, my_disk);//copy content of my_disk
	list_index = find_file_disk(MYDSK,source_file);//find file name on the disk
	if (list_index == -1 ){printf("file not found\n");return;}//if it cannot find, then close
	FILE *source = fopen(destination_file,"ab+");//if it founds open the destinaiton file to write on it
	
	file_size = MYDSK.File_list.items[list_index].fileSize;//copy file info
	fat_index = MYDSK.File_list.items[list_index].firstBlock;

	unsigned int chunk_count = (file_size/512)+1;//calcuate chunk number
	unsigned char buffer[chunk_count][LENGTH_OF_DATA];//define buffer according to chunk number

	for (int i = 0; i < chunk_count; i++)//count to number of blocks
	{
		memcpy(&buffer[i],&MYDSK.TotData.data[fat_index],sizeof(MYDSK.TotData.data[fat_index]));//fill buffer according to data in disk.image
		fat_index=bigToLittleEndian(MYDSK.fat.table[fat_index]);
	}
	fseek(source, 0, SEEK_SET);
	fwrite(&buffer,file_size,1,source);//write on it
	fclose(source);//and close
}
void delete(char* source_file)
{
	disk MYDSK;

	unsigned int file_size, fat_index, list_index,next_index;
	
	fread(&MYDSK, sizeof(MYDSK), 1, my_disk);//copy content of my_disk
	list_index = find_file_disk(MYDSK,source_file);
	if (list_index == -1 ){printf("file not found\n");return;}
	file_size = MYDSK.File_list.items[list_index].fileSize;
	fat_index = MYDSK.File_list.items[list_index].firstBlock;

	unsigned int chunk_count = (file_size/512)+1;
	unsigned char buffer[chunk_count][LENGTH_OF_DATA];

	MYDSK.File_list.items[list_index].firstBlock = 0;//make it 0
	MYDSK.File_list.items[list_index].fileSize = 0;
	strcpy(MYDSK.File_list.items[list_index].fileName,"");

	for (int i = 0; i < chunk_count; i++)//count to number of blocks
	{
		next_index = bigToLittleEndian(MYDSK.fat.table[fat_index]);//firstly get next data
		MYDSK.fat.table[fat_index] = 0;//then make it equal to zero
		memset(&MYDSK.TotData.data[fat_index],0,sizeof(MYDSK.TotData.data[fat_index]));//set it 0
		fat_index=next_index;
	}
	fseek(my_disk, 0, SEEK_SET);
	fwrite(&MYDSK, sizeof(MYDSK), 1, my_disk);
	fseek(my_disk, 0, SEEK_SET);
}
void print_file_list_to_the_file(){//same as list but I used fprintf instead of printf
	FILE * destination = fopen("filelist.txt","wb+");
	disk MYDSK;
	fread(&MYDSK, sizeof(MYDSK), 1, my_disk);
	int count=0;
	int name_lenght;
	fprintf(destination,"file name");
	for (int i = 0; i < 30-strlen("file name"); i++)
	{
		fprintf(destination," ");
	}
	fprintf(destination,"file size\n");
	for (int i = 0; i < LENGTH_OF_FILE_LIST; i ++) {
		if(MYDSK.File_list.items[i].fileSize != 0)//If it is 0,then do not write
		{
			count++;
			fprintf(destination,"%s",MYDSK.File_list.items[i].fileName);
			name_lenght = strlen(MYDSK.File_list.items[i].fileName);
			for (int i = 0; i < 30-name_lenght; i++)
			{
				fprintf(destination," ");
			}
			fprintf(destination,"%ld\n", MYDSK.File_list.items[i].fileSize);
			//printf("%d\n", File_list.items[i].firstBlock);
		}
    }
	if (count == 0)
	{
		printf("Disk is empty...\n");
	}
	fclose(destination);
}
void print_fat_to_the_file(){
	fseek(my_disk, 0, SEEK_SET);
	disk MYDSK;
	fread(&MYDSK, sizeof(MYDSK), 1, my_disk);//copy content of data in disk.image
	FILE * destination = fopen("fat.txt","wb+");
	for (int i = 0; i < LENGTH_OF_FAT; i++) {
        fprintf(destination,"%08X ", MYDSK.fat.table[i]);
        if ((i+1) % 16 == 0)//every 16 data, print space
        {
            fprintf(destination,"\n");
        }
    }
	fclose(destination);
	
}
int count_files(disk MYDSK){//counts file number in any disk
	int count=0;
	for (int i = 0; i < LENGTH_OF_FILE_LIST; i++)
	{
		if (MYDSK.File_list.items[i].fileSize != 0)
		{
			count++;
		}
	}
	return count;
}
void defragmentation(){
	disk MYDSK;
	disk MYDSK_temp;
	fseek(my_disk, 0, SEEK_SET);
	fread(&MYDSK_temp, sizeof(MYDSK_temp), 1, my_disk);//copy content of disk.iamge
	fseek(my_disk, 0, SEEK_SET);
	int file_count = count_files(MYDSK_temp);//counts file numbers
	MYDSK.fat.table[0] = 0xFFFFFFFF;
	unsigned int chunk_count,first_fat_index,file_list_index,previous_index=0,current_index=0,temp_index,list_index,count = 0;
	if (file_count == 0)
	{
		return;
	}
	for (int i = 0; i < LENGTH_OF_FILE_LIST; i++)//check all files
	{
		if (0 == MYDSK_temp.File_list.items[i].fileSize)//if file size is 0 then skip
		{
			continue;
		}
		if (count == file_count)
		{
			return;//if we get all data as much as count that we should gather
		}
		strcpy(MYDSK.File_list.items[count].fileName,MYDSK_temp.File_list.items[i].fileName);//copy file name to the first index
		MYDSK.File_list.items[count].fileSize = MYDSK_temp.File_list.items[i].fileSize;//copy file size to the first index
		
		temp_index = MYDSK_temp.File_list.items[i].firstBlock;//copy first block to the first index
		chunk_count = (MYDSK.File_list.items[count].fileSize/512)+1;//calculate chunk size
		for (int i = 0; i < chunk_count; i++)
		{//do same things in read funciton but gather old data to the new temp disk
			if (i == 0)//placed to the File_list
			{
				first_fat_index = nearest_empty_index(MYDSK.fat);
				MYDSK.File_list.items[count].firstBlock = first_fat_index;//copy it according to count value
				previous_index = first_fat_index;
				MYDSK.fat.table[previous_index] = 0x12345678;//trivial data to make this chunk is filled
				memcpy(&MYDSK.TotData.data[first_fat_index],&MYDSK_temp.TotData.data[temp_index],sizeof(data_chunk));//copy first chunk of data
			}
			else if (i == (chunk_count-1))//Make Last Fat table place as 
			{
				temp_index =bigToLittleEndian( MYDSK_temp.fat.table[temp_index]);
				current_index = nearest_empty_index(MYDSK.fat);
				MYDSK.fat.table[previous_index] = bigToLittleEndian(current_index);
				current_index = nearest_empty_index(MYDSK.fat);
				MYDSK.fat.table[current_index] = 0xFFFFFFFF;
				memcpy(&MYDSK.TotData.data[current_index],&MYDSK_temp.TotData.data[temp_index],sizeof(data_chunk));
			}
			else if ((i != chunk_count-1) && (i>0))//Make Last Fat table place as 
			{
				temp_index =bigToLittleEndian( MYDSK_temp.fat.table[temp_index]);
				//temp_index =MYDSK_temp.fat.table[temp_index];
				current_index = nearest_empty_index(MYDSK.fat);
				MYDSK.fat.table[previous_index] = bigToLittleEndian(current_index);
				MYDSK.fat.table[current_index] = 0x12345678;//trivial data to make this chunk is filled
				memcpy(&MYDSK.TotData.data[current_index],&MYDSK_temp.TotData.data[temp_index],sizeof(data_chunk));
				previous_index = current_index;
			}		
		}
		count ++;	
	}
	fseek(my_disk, 0, SEEK_SET);
	fwrite(&MYDSK, sizeof(MYDSK), 1, my_disk);//write new disk 
	fseek(my_disk, 0, SEEK_SET);
}
void swap(File_List_Item *a, File_List_Item *b) {//swap two file list item
    File_List_Item temp = *a;
    *a = *b;
    *b = temp;
}
void sortBySizeDescending(fileList *list, int numFiles) {//sort by descending items in file list 
    for (int i = 0; i < numFiles - 1; i++) {
        for (int j = 0; j < numFiles - i - 1; j++) {
            if (list->items[j].fileSize < list->items[j + 1].fileSize) {
                swap(&list->items[j], &list->items[j + 1]);//change order of two filelist item
            }
        }
    }
}
void sortBySizeAscending(fileList *list, int numFiles) {//sort by ascending items in file list 
    for (int i = 0; i < numFiles - 1; i++) {
        for (int j = 0; j < numFiles - i - 1; j++) {
            if (list->items[j].fileSize > list->items[j + 1].fileSize) {
                swap(&list->items[j], &list->items[j + 1]);
            }
        }
    }
}
void sortA(){
	disk MYDSK;
	fseek(my_disk, 0, SEEK_SET);
	fread(&MYDSK, sizeof(MYDSK), 1, my_disk);
	fseek(my_disk, 0, SEEK_SET);
	int count = count_files(MYDSK);
	int files_index_ascending[count];
	fileList temp_list;
	memcpy(&temp_list,&MYDSK.File_list,sizeof(temp_list));
	sortBySizeAscending(&temp_list,LENGTH_OF_FILE_LIST);//sort file list that get from disk
	print_file_list(temp_list);//then print it
}
void sortD(){
	disk MYDSK;
	fseek(my_disk, 0, SEEK_SET);
	fread(&MYDSK, sizeof(MYDSK), 1, my_disk);
	fseek(my_disk, 0, SEEK_SET);
	int count = count_files(MYDSK);
	int files_index_ascending[count];
	fileList temp_list;
	memcpy(&temp_list,&MYDSK.File_list,sizeof(temp_list));
	sortBySizeDescending(&temp_list,LENGTH_OF_FILE_LIST);//sort file list that get from disk
	print_file_list(temp_list);//then print it
}
int main(int argc, char *argv[]) {
	my_disk = fopen(argv[1],"r+");
	if (argc > 2)
	{
		if(strcmp(argv[2],"-format")==0){//Format()
		Format();
		}

		else if (strcmp(argv[2],"-write")==0){//Write()
			write(argv[3],argv[4]);
		}
		else if (strcmp(argv[2],"-read")==0){//Write()
			read(argv[3],argv[4]);
		}
		else if (strcmp(argv[2],"-list")==0){//Read()
			List();
		}
		else if (strcmp(argv[2],"-delete")==0){//Read()
			delete(argv[3]);
		}
		else if (strcmp(argv[2],"-printfat")==0){//Delete()
			print_fat_to_the_file();
		}
		else if (strcmp(argv[2],"-printfilelist")==0){//List()
			print_file_list_to_the_file();
		}
		else if (strcmp(argv[2],"-defragment")==0){//PrintFileList()
			defragmentation();		
		}
		else if (strcmp(argv[2],"-sorta")==0){//PrintFat()
			sortA();
		}
		else if (strcmp(argv[2],"-sortd")==0){//Defragment()
			sortD();
		}
		else
			printf("UNKOWN COMMENT..\n");//Unkown comment is entered

	}
    return 0;
}
