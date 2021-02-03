#ifndef DATABASE_H
#define DATABASE_H


MYSQL* __GetMysqlCon(void);

void __InitDatabaseConnection(void);
void __FreeDatabaseConnection(void);
void __AnormalAbortCon(char const*);
void __PrintDError(char const*);
bool __GetSMysqlConStatus(void);

#endif
