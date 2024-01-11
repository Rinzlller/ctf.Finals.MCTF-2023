# Wolf_And_Eggs sploits

1. [EASY] В бинарнике есть backdoor функция, которая весит на опции 1337.
	```c
	void __fastcall backdoor()
		{
		  system("/bin/sh");
		}
	```

	#### ATTACK:
		1. Регистрируемся за пользователя;
		2. Проходим аутентификацию за пользователя;
		3. Запускаем shell на опции 1337;
	
	#### FIX:
    + Через LD_PRELOAD переписать system() на пустую
    + Через IDA переписать строку "/bin/sh", на "/bi//sh" (например)
    + Через IDA заполнить функцию backdoor() командами NOP с опкодом 0x90

2. [HARD] Use-After-Free + tcache poisining + got rewrite.
	```c
	void delete_egg()
	{
	  ...
	  free(eggs_array[egg_id]->egg_name);
	  //eggs_array[egg_id]->egg_name = NULL;
	  free(eggs_array[egg_id]->egg_data);
	  //eggs_array[egg_id]->egg_data = NULL;
	  free(eggs_array[egg_id]);
	  //eggs_array[egg_id] = NULL;
	}
	```

	#### FIX:
+ Через IDA изменить входные данные функции free с *ptr на **ptr и через LD_PRELOAD добавить во free() обнуление по адресу *ptr после основног free(*ptr)

    Изменения через IDA
	```
	48 8B 40 08                             mov     rax, [rax+8]
	↓  ↓  ↓  ↓                              ↓       ↓    ↓
	48 83 c0 08                             add     rax, 8
    ...                                     ...
	48 8B 40 10                             mov     rax, [rax+10h]
	↓  ↓  ↓  ↓                              ↓       ↓    ↓
	48 83 c0 10                             add     rax, 10h
    ...                                     ...
	48 8B 04 02                             mov     rax, [rdx+rax]
	↓  ↓  ↓  ↓                              ↓       ↓    ↓
	48 01 d0 90                             add     rax, rdx
	                                        nop
	```
	приводят к следующему
	```c
	free(eggs_array[egg_id]->egg_name);
	free(eggs_array[egg_id]->egg_data);
	free(eggs_array[egg_id]);
	↓  ↓  ↓  ↓  ↓  ↓  ↓  ↓  ↓  ↓  ↓  ↓
	free(&eggs_array[egg_id]->egg_name);
	free(&eggs_array[egg_id]->egg_data);
	free(&eggs_array[egg_id]);
	```
    После этого остаётся подложить следующую библиотеку на подгрузку исправленному бинарю перед основной libc.so.6: 
	```c
	#define _GNU_SOURCE
	#include <dlfcn.h>
	#include <stdio.h>


	typedef int (*orig_func_type)(void *ptr);

	void free(void **ptr)
	{
	    orig_func_type orig_free;
	    orig_free = (orig_func_type)dlsym(RTLD_NEXT,"free");
	    orig_free(*ptr);

	    *ptr = NULL;
	}
	```
    P.S. Важные замечания:
	1. Скомпилировать библиотеку можно командой ниже. Также не помешает проверить работу библиотеки на тестовой программе, как, например, test.c.
    	```bash
    	gcc -ldl -shared -fPIC ./patch.c -o patch.so
    	```
    2. Подложить на подгрузку бинарю при помощи LD_PRELOAD в нужно server.sh вот так (иногда вместо запятой необходимо использовать двоеточие):
    	```bash
    	sudo -u root env LD_PRELOAD="/task/patch.so,/task/libc.so.6" /task/vuln'
    3. И обязтельно нужно правильно сохранить бинарь после правки через IDA и не забыть заменить его в папках, где собирается образ сервиса. Для уточнения можно сравнить md5sum бинаря внутри контейнера и исправленного через IDA.
    	```bash
    	docker exec -it wolf_and_eggs-wolfeggs-1 md5sum vuln; \
    	md5sum vuln 
    	```