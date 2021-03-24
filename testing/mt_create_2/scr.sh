for i in {1..10}
do
	folder="size_$i"
	mkdir $folder
	filename="$i.c"
	touch $folder/$filename
	
	for ((j=1; j<$i + 10; j++))
	do
		echo -n "sym-test-" >> $folder/$filename
	done
done
