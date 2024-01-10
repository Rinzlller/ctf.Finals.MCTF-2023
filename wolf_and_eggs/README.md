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
	  free(eggs_array[egg_id]->egg_data);
	  free(eggs_array[egg_id]);
	  //eggs_array[egg_id] = NULL;
	}
	```
