for i in {1..10}
do
	folder="size_$i"
	mkdir $folder
	filename="size$i"_"test$((11-$i)).c"
	touch $folder/$filename
	
	for ((j=1; j<=$i; j++))
	do
		echo -n "sym-" >> $folder/$filename
		echo -n "sym-" >> $folder/$filename
		echo -n "sym-" >> $folder/$filename
		echo -n "sym-" >> $folder/$filename
	done
		for ((j=10; j >= i; j--))
	do
		echo -n "-test" >> $folder/$filename
	done
done
