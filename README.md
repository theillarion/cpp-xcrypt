# cpp-xcrypt
## Summary
Небольшая криптографическая бибилиотека для факторизации целых чисел
## Features
  * Факторизация целых чисел (100 бит) в разумные пределы времени.
  * Некоторые побочные алгоритмы, такие как:
    * Обычный и расширенный алгоритм Евклида.
    * Быстрое возведение в степень по модулю.
    * Вычисление символа Лежандра.
    * Вероятностный тест Миллера-Рабина.
    * Решение СЛАУ в GF(2).
## Solution
Использует алгоритм квадратичного решета:
  * **1 этап** - создание факторной базы. Это такие простые числа, которые образуют квадратичный вычет по модулю n (факторизуемое число), и не превышают заданную границу B.
  * **2 этап** - просеивание или получение B-гладких чисел. B-гладкие число это такое число, у которого все простые делители не превосходят заданную границу B. Используется неэффективное решение, которое не может работать параллельно в нескольких потоках.
  * **3 этап** - решение СЛАУ в GF(2). Также используется неэффективное решение методом Гаусса.
  * **4 этап** - нахождение нетривиальных делителей среди решений СЛАУ.

## Requirements
  * С++20
  * CMake 3.16
  * Библиотека длинной арифметики. Желательно Boost.

## Build
Библиотека поставляется в виде заголовков, тем самым, достаточно подключить необходимый заголовочный файл и указать компилятору путь к директории *include*. Также, можно подключить как подпроект в cmake файле.

## Problems
:hourglass_flowing_sand: Ручной подбор параметров B и L. Необходимо реализовать автоматический подбор.\
:hourglass_flowing_sand: Просеивание не может выполняться в многопоточном режиме, необходимо переписать алгоритм.\
:hourglass_flowing_sand: Решение СЛАУ использует метод Гаусса. Учитывая, что поле конечно и вовсе двоичное, то существуют более специализированные и эффективные алгоритмы.