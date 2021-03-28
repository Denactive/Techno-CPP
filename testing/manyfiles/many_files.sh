for i in {1..500}
do
	touch cases/"$i".c
	test_amount=$((1 + $i));
	for ((j=1; j < $test_amount; j++))
	do
		echo -n "$i" + "test " >> cases/"$i".c
	done
done
