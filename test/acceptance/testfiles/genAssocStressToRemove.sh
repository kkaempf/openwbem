for i in `seq $1`; do
	for j in `seq $2`; do
		echo "INSTANCE OF Test_Assoc { Endpoint1=\"Test_EndpointClass1.key=\\\"${i} some extra junk to use up space\\\"\"; Endpoint2=\"Test_EndpointClass2.key=\\\"${j} other foo bar baz stuff\\\"\"; };"
	done
done

for j in `seq $2`; do
	echo "INSTANCE OF Test_EndpointClass2 { key=\"${j} other foo bar baz stuff\"; };"
done

for i in `seq $1`; do
	echo "INSTANCE of Test_EndpointClass1 { key=\"${i} some extra junk to use up space\"; };"
done

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

