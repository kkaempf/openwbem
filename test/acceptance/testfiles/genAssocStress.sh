echo "class Test_EndpointClass1 {"
echo "	[key] string key;"
echo "};"
echo "class Test_EndpointClass2 {"
echo "	[key] string key;"
echo "};"
echo "[Association]"
echo "class Test_Assoc {"
echo "	[key] Test_EndpointClass1 REF Endpoint1;"
echo "	[key] Test_EndpointClass2 REF Endpoint2;"
echo "};"

for i in `seq $1`; do
	echo "INSTANCE of Test_EndpointClass1 as \$c1${i} { key=\"${i} some extra junk to use up space\"; };"
done

for j in `seq $2`; do
	echo "INSTANCE OF Test_EndpointClass2 as \$c2${j} { key=\"${j} other foo bar baz stuff\"; };"
done

for i in `seq $1`; do
	for j in `seq $2`; do
		echo "INSTANCE OF Test_Assoc { Endpoint1=\$c1${i}; Endpoint2=\$c2${j}; };"
	done
done

