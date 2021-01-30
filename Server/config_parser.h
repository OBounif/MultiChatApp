#ifndef CONFIG_PARSER
#define CONFIG_PARSER


#ifdef  CONFIG
#define BUFFER_LENGTH 255 
#endif

typedef struct Map 
{
	char* key;
	void* data;
	size_t *data_length;
	struct Map* next;
}Map;


void __ParseCfgFile(char const*);
void* __Get(char const*);
void __FreePData(void);



#endif
