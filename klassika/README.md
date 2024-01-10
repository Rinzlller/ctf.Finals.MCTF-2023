# Klassika sploits

1. У поля id в таблице Users нет AUTOINCREMENT => можно переиспользовать.

## ATTACK:
	1. Создаём пользователей с id 1, 2 и 3 (например);
	2. Запоминаем cookie для id 1, 2 и 3;
	3. Читирим и попадаем ими в бан => эти id освобождаются в БД;
	4. Следующие usual пользователи получат id = 1, 2 и 3;
	5. Заходим в их профили, используя сохранённые ранее cookie;

## FIX:
	+ Выставить полю id тип AUTOINCREMENT:
```sql
CREATE TABLE Users_fix (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	username TEXT NOT NULL,
	password TEXT NOT NULL
);
INSERT INTO Users_fix SELECT * FROM Users;
DROP TABLE Users;
ALTER TABLE Users_fix RENAME TO Users;
```

2. Страница news х2 раза рендерится перед выводом пользователю => SSTI в поле username.
P.S. Использовать кавычки не получится из-за html-encoding

## ATTACK:
	1. Создаём пользователей с ssti-нагрузкой в username;
	2. Читирим и попадаем в бан (и новости);
	3. Открываем новости и читаем флаги;

## FIX:
	+ Убрать лишнюю функцию render().

3. [CVE-2017-11424](https://github.com/silentsignal/rsa_sign2n/tree/release/CVE-2017-11424) Используется уязвимая версия pyjwt. По двум jwt можно восстановить публичный ключ и подписывать свои jwt, изменив алгоритм с асимметричного на симметричный.

## ATTACK:
	1. Создаём 2-x пользователей;
	2. Через их jwt определяем pub ключ;
	3. Генерируем новые jwt с актульаными user_id, изменив алгоритм и подписав их только pub ключом;
	4. Просматриваем профили пользователей;

## FIX:
	+ Повысить версию pyjwt до ^2.4.0 и указать алгоритм для decode();
	+ Только указать алгоритм для decode():
```python
jwt.decode(request.cookies['session'], public_key, algorithms="RS256")
```