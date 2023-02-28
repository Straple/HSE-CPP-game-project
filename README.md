# DOCUMENTATION

# ASSERT(condition, message)

Макрос. Проверяет, что условие верно. Если это не так, то выводит сообщение об ошибке и завершает работу. Полезен, так как в играх много инвариантов и их все необходимо учитывать.

example

![image](https://user-images.githubusercontent.com/54230867/221960861-44a337e2-8109-4bbc-bec7-6ffaf688fd71.png)
![image](https://user-images.githubusercontent.com/54230867/221961266-88e8071a-dbe1-4d95-a4fd-f938d488a290.png)

implementation in assert.h

![image](https://user-images.githubusercontent.com/54230867/221961669-9f15675f-189c-4cac-a03d-3228bf70807f.png)

# clamp(min, val, max)

Возвращает значение между min и max. Удобно, так как можно например легко удерживать позицию предмета между некоторыми границами. Должно быть выполнено min <= max, иначе упадет ассерт.

implementation in utils.h

![image](https://user-images.githubusercontent.com/54230867/221964455-a50ea4b8-88a8-48b2-8700-a02ebfff701c.png)

# is_between(min, val, max)

Проверяет, что val лежит между min и max. Удобен для читабельности кода, так как не нужно дважды писать вычисление val (вдруг это формула какая-то) и писать <=. Должно быть выполнено min <= max, иначе упадет ассерт.

implementation in utils.h

![image](https://user-images.githubusercontent.com/54230867/221968706-325d1734-db78-491e-9db5-33f7fda05a4e.png)


# to_string(val)

Обертка над std::to_string(val). Нужна чтобы переопределить to_string для efloat и выписывать его с определенной точностью для однообразности. Используется для отрисовки плавающих чисел на экран.

implementation in utils.h

![image](https://user-images.githubusercontent.com/54230867/221967929-e8d29afd-95fe-4598-b80d-28086af892cf.png)

