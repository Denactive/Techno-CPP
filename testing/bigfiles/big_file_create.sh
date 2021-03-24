for i in {9..50}
do
	touch cases/"$i".c
	test_amount=$((1000 + $i));
	for ((j=1; j < $test_amount; j++))
	do
		for ((k = 1; k < 1001; k++))
		do
			echo -n "a" >> cases/"$i".c
		done
		echo -n "test" >> cases/"$i".c
	done
done
