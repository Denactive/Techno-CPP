# [Techno] CPP
Домашки Турчина Дениса

Тест скорости проводил на 26 файлах 1 мБ

Speed test | Multi-thread
	4 Threads:
	execution time: 1.867002
	5 Threads:
	execution time: 1.544632
	6 Threads:
	execution time: 1.308196
	7 Threads:
	execution time: 1.142795
	8 Threads:
	execution time: 0.974311
	9 Threads:
	execution time: 0.844081
	10 Threads:
	execution time: 0.793779
	11 Threads:
	execution time: 0.706832
	12 Threads:
	execution time: 0.665971
	13 Threads:
	execution time: 0.614233
	14 Threads:
	execution time: 0.568251
	15 Threads:
	execution time: 0.532881
	16 Threads:
	execution time: 0.483077
	
Speed test | Single-Thread
	execution time: 0.181277


ИЗ2 посвящено приобретению навыков системной разработки на C и работе с внешними библиотеками. В качестве результата ИЗ2 ожидается:
* грамотное разбиение проекта на файлы;
* наличие двух реализаций – последовательной и параллельной, оформленных в виде статической и динамической библиотеки соответственно, а также тестирующей программы, которая будет сравнивать на разных входных данных результаты обеих реализаций между собой;
* максимальная утилизация ресурсов процессора при параллельной обработке данных путём использования нескольких процессов или потоков;
* продуманные структуры данных в целях экономии оперативной памяти;
* реализация алгоритмов, эффективно взаимодействующих с кэш-памятью.
 
# Вариант #74
На вход поступает путь до директории проекта на языке С. Проект может быть любым, но для того, чтобы протестировать эффективность распараллеливания – имеет смысл взять довольно объёмный проект – например, скачав его из списка популярных репозиториев на языке C (https://github.com/trending/c). Также на вход поступает слово, по которому необходимо осуществить поиск по всем файлам проекта. В результате выполнения программа должна вывести отсортированный список файлов, содержащих искомое слово. Критерий сортировки – по убыванию количества вхождений слова в файл. Поиск необходимо осуществлять независимо от регистра, только по файлам .c, причём необходимо игнорировать текст комментариев. Распараллеливание осуществить на уровне использования нескольких потоков.

На что необходимо обратить внимание:
- основная информация описана в https://park.mail.ru/blog/topic/view/14270/
- параллельная реализация не должна быть осуществлена с помощью процессов, когда требуется реализация с помощью потоков (и наоборот);
- компиляция должна происходить с флагами -Wall -Werror -Wpedantic, то есть необработанных ворнингов быть не должно;
- количество потоков/процессов должно быть не захардкожено, а определяться в зависимости от возможностей системы (например, в зависимости от количества ядер процессора);
- при разработке обеих библиотек стоит делать общий интерфейс, не раскрывая особенностей реализации;
- библиотеки должны быть взаимозаменяемыми - конкретная реализация (последовательная/параллельная) - использоваться в зависимости от конфигурации сборки;
- юнит-тесты должны быть реализованы для обеих реализаций (последовательной/параллельной). Покрытие тестами должно быть максимально возможным;
- должны присутствовать стресс-тесты. Они могут быть реализованы внешним образом, запуская две разные программы - одну со статической библиотекой с последовательной реализацией, вторую - с динамической библиотекой с параллельной реализацией, и сравнивая их выводы друг с другом.
- для организации ввода/вывода больших данных полезно работать с файлами, а в программе - предусмотреть работу с универсальными потоками входных/выходных данных (или хотя бы перенаправлять ввод/вывод на уровне их запуска)
- если в задании сказано, что программа должна обрабатывать файлы объёмом 100 Мб – это лишь ориентир, на которых программа точно должна работать, и на котором имеет смысл делать замеры производительности и эффективности алгоритмов. Поэтому тесты на такой объём должны быть. Однако сама программа должна уметь работать с произвольными размерами входных данных
- измерение времени должно осуществляться внешним образом, а не внутри кода библиотек. При этом необходимо делать несколько замеров и усреднять. Стоит помнить о том, что clock() в многопоточных приложениях работает не так, как ожидается.
