# y2k regression tests for the ls utility

export LC_ALL=C TZ=EST5EDT

listformat='%(mtime:time=%K)s %(path)s'

VIEW data y2k.dat

function DATA
{
	case $1 in
	pax)	pax --nosummary --from=ascii -rf $data
		;;
	sizes)	f=f
		x=x
		while	:
		do	case $f in
			fffffffffff)	break ;;
			esac
			echo $x > $f
			f=f$f
			x=$x$x
		done
		;;
	esac
}

TEST 01 'down to the second'

	DO DATA pax

	EXEC --format="$listformat" *.dat
		SAME OUTPUT list.dat

	DO touch x y z

	EXEC -w24x80 -C [a-z]*
		OUTPUT - $'f00000000.dat  f00000075.dat  f00000150.dat  f00000225.dat  f00000300.dat
f00000001.dat  f00000076.dat  f00000151.dat  f00000226.dat  f00000301.dat
f00000002.dat  f00000077.dat  f00000152.dat  f00000227.dat  f00000302.dat
f00000003.dat  f00000078.dat  f00000153.dat  f00000228.dat  f00000303.dat
f00000004.dat  f00000079.dat  f00000154.dat  f00000229.dat  f00000304.dat
f00000005.dat  f00000080.dat  f00000155.dat  f00000230.dat  f00000305.dat
f00000006.dat  f00000081.dat  f00000156.dat  f00000231.dat  f00000306.dat
f00000007.dat  f00000082.dat  f00000157.dat  f00000232.dat  f00000307.dat
f00000008.dat  f00000083.dat  f00000158.dat  f00000233.dat  f00000308.dat
f00000009.dat  f00000084.dat  f00000159.dat  f00000234.dat  f00000309.dat
f00000010.dat  f00000085.dat  f00000160.dat  f00000235.dat  f00000310.dat
f00000011.dat  f00000086.dat  f00000161.dat  f00000236.dat  f00000311.dat
f00000012.dat  f00000087.dat  f00000162.dat  f00000237.dat  f00000312.dat
f00000013.dat  f00000088.dat  f00000163.dat  f00000238.dat  f00000313.dat
f00000014.dat  f00000089.dat  f00000164.dat  f00000239.dat  f00000314.dat
f00000015.dat  f00000090.dat  f00000165.dat  f00000240.dat  f00000315.dat
f00000016.dat  f00000091.dat  f00000166.dat  f00000241.dat  f00000316.dat
f00000017.dat  f00000092.dat  f00000167.dat  f00000242.dat  f00000317.dat
f00000018.dat  f00000093.dat  f00000168.dat  f00000243.dat  f00000318.dat
f00000019.dat  f00000094.dat  f00000169.dat  f00000244.dat  f00000319.dat
f00000020.dat  f00000095.dat  f00000170.dat  f00000245.dat  f00000320.dat
f00000021.dat  f00000096.dat  f00000171.dat  f00000246.dat  f00000321.dat
f00000022.dat  f00000097.dat  f00000172.dat  f00000247.dat  f00000322.dat
f00000023.dat  f00000098.dat  f00000173.dat  f00000248.dat  f00000323.dat
f00000024.dat  f00000099.dat  f00000174.dat  f00000249.dat  f00000324.dat
f00000025.dat  f00000100.dat  f00000175.dat  f00000250.dat  f00000325.dat
f00000026.dat  f00000101.dat  f00000176.dat  f00000251.dat  f00000326.dat
f00000027.dat  f00000102.dat  f00000177.dat  f00000252.dat  f00000327.dat
f00000028.dat  f00000103.dat  f00000178.dat  f00000253.dat  f00000328.dat
f00000029.dat  f00000104.dat  f00000179.dat  f00000254.dat  f00000329.dat
f00000030.dat  f00000105.dat  f00000180.dat  f00000255.dat  f00000330.dat
f00000031.dat  f00000106.dat  f00000181.dat  f00000256.dat  f00000331.dat
f00000032.dat  f00000107.dat  f00000182.dat  f00000257.dat  f00000332.dat
f00000033.dat  f00000108.dat  f00000183.dat  f00000258.dat  f00000333.dat
f00000034.dat  f00000109.dat  f00000184.dat  f00000259.dat  f00000334.dat
f00000035.dat  f00000110.dat  f00000185.dat  f00000260.dat  f00000335.dat
f00000036.dat  f00000111.dat  f00000186.dat  f00000261.dat  f00000336.dat
f00000037.dat  f00000112.dat  f00000187.dat  f00000262.dat  f00000337.dat
f00000038.dat  f00000113.dat  f00000188.dat  f00000263.dat  f00000338.dat
f00000039.dat  f00000114.dat  f00000189.dat  f00000264.dat  f00000339.dat
f00000040.dat  f00000115.dat  f00000190.dat  f00000265.dat  f00000340.dat
f00000041.dat  f00000116.dat  f00000191.dat  f00000266.dat  f00000341.dat
f00000042.dat  f00000117.dat  f00000192.dat  f00000267.dat  f00000342.dat
f00000043.dat  f00000118.dat  f00000193.dat  f00000268.dat  f00000343.dat
f00000044.dat  f00000119.dat  f00000194.dat  f00000269.dat  f00000344.dat
f00000045.dat  f00000120.dat  f00000195.dat  f00000270.dat  f00000345.dat
f00000046.dat  f00000121.dat  f00000196.dat  f00000271.dat  f00000346.dat
f00000047.dat  f00000122.dat  f00000197.dat  f00000272.dat  f00000347.dat
f00000048.dat  f00000123.dat  f00000198.dat  f00000273.dat  f00000348.dat
f00000049.dat  f00000124.dat  f00000199.dat  f00000274.dat  f00000349.dat
f00000050.dat  f00000125.dat  f00000200.dat  f00000275.dat  f00000350.dat
f00000051.dat  f00000126.dat  f00000201.dat  f00000276.dat  f00000351.dat
f00000052.dat  f00000127.dat  f00000202.dat  f00000277.dat  f00000352.dat
f00000053.dat  f00000128.dat  f00000203.dat  f00000278.dat  f00000353.dat
f00000054.dat  f00000129.dat  f00000204.dat  f00000279.dat  f00000354.dat
f00000055.dat  f00000130.dat  f00000205.dat  f00000280.dat  f00000355.dat
f00000056.dat  f00000131.dat  f00000206.dat  f00000281.dat  f00000356.dat
f00000057.dat  f00000132.dat  f00000207.dat  f00000282.dat  f00000357.dat
f00000058.dat  f00000133.dat  f00000208.dat  f00000283.dat  f00000358.dat
f00000059.dat  f00000134.dat  f00000209.dat  f00000284.dat  f00000359.dat
f00000060.dat  f00000135.dat  f00000210.dat  f00000285.dat  f00000360.dat
f00000061.dat  f00000136.dat  f00000211.dat  f00000286.dat  f00000361.dat
f00000062.dat  f00000137.dat  f00000212.dat  f00000287.dat  f00000362.dat
f00000063.dat  f00000138.dat  f00000213.dat  f00000288.dat  f00000363.dat
f00000064.dat  f00000139.dat  f00000214.dat  f00000289.dat  f00000364.dat
f00000065.dat  f00000140.dat  f00000215.dat  f00000290.dat  f00000365.dat
f00000066.dat  f00000141.dat  f00000216.dat  f00000291.dat  f00000366.dat
f00000067.dat  f00000142.dat  f00000217.dat  f00000292.dat  f00000367.dat
f00000068.dat  f00000143.dat  f00000218.dat  f00000293.dat  f00000368.dat
f00000069.dat  f00000144.dat  f00000219.dat  f00000294.dat  f00000369.dat
f00000070.dat  f00000145.dat  f00000220.dat  f00000295.dat  f00000370.dat
f00000071.dat  f00000146.dat  f00000221.dat  f00000296.dat  list.dat
f00000072.dat  f00000147.dat  f00000222.dat  f00000297.dat  x
f00000073.dat  f00000148.dat  f00000223.dat  f00000298.dat  y
f00000074.dat  f00000149.dat  f00000224.dat  f00000299.dat  z'

	DO touch w

	EXEC -w24x80 -C [a-z]*
		OUTPUT - $'f00000000.dat  f00000076.dat  f00000152.dat  f00000228.dat  f00000304.dat
f00000001.dat  f00000077.dat  f00000153.dat  f00000229.dat  f00000305.dat
f00000002.dat  f00000078.dat  f00000154.dat  f00000230.dat  f00000306.dat
f00000003.dat  f00000079.dat  f00000155.dat  f00000231.dat  f00000307.dat
f00000004.dat  f00000080.dat  f00000156.dat  f00000232.dat  f00000308.dat
f00000005.dat  f00000081.dat  f00000157.dat  f00000233.dat  f00000309.dat
f00000006.dat  f00000082.dat  f00000158.dat  f00000234.dat  f00000310.dat
f00000007.dat  f00000083.dat  f00000159.dat  f00000235.dat  f00000311.dat
f00000008.dat  f00000084.dat  f00000160.dat  f00000236.dat  f00000312.dat
f00000009.dat  f00000085.dat  f00000161.dat  f00000237.dat  f00000313.dat
f00000010.dat  f00000086.dat  f00000162.dat  f00000238.dat  f00000314.dat
f00000011.dat  f00000087.dat  f00000163.dat  f00000239.dat  f00000315.dat
f00000012.dat  f00000088.dat  f00000164.dat  f00000240.dat  f00000316.dat
f00000013.dat  f00000089.dat  f00000165.dat  f00000241.dat  f00000317.dat
f00000014.dat  f00000090.dat  f00000166.dat  f00000242.dat  f00000318.dat
f00000015.dat  f00000091.dat  f00000167.dat  f00000243.dat  f00000319.dat
f00000016.dat  f00000092.dat  f00000168.dat  f00000244.dat  f00000320.dat
f00000017.dat  f00000093.dat  f00000169.dat  f00000245.dat  f00000321.dat
f00000018.dat  f00000094.dat  f00000170.dat  f00000246.dat  f00000322.dat
f00000019.dat  f00000095.dat  f00000171.dat  f00000247.dat  f00000323.dat
f00000020.dat  f00000096.dat  f00000172.dat  f00000248.dat  f00000324.dat
f00000021.dat  f00000097.dat  f00000173.dat  f00000249.dat  f00000325.dat
f00000022.dat  f00000098.dat  f00000174.dat  f00000250.dat  f00000326.dat
f00000023.dat  f00000099.dat  f00000175.dat  f00000251.dat  f00000327.dat
f00000024.dat  f00000100.dat  f00000176.dat  f00000252.dat  f00000328.dat
f00000025.dat  f00000101.dat  f00000177.dat  f00000253.dat  f00000329.dat
f00000026.dat  f00000102.dat  f00000178.dat  f00000254.dat  f00000330.dat
f00000027.dat  f00000103.dat  f00000179.dat  f00000255.dat  f00000331.dat
f00000028.dat  f00000104.dat  f00000180.dat  f00000256.dat  f00000332.dat
f00000029.dat  f00000105.dat  f00000181.dat  f00000257.dat  f00000333.dat
f00000030.dat  f00000106.dat  f00000182.dat  f00000258.dat  f00000334.dat
f00000031.dat  f00000107.dat  f00000183.dat  f00000259.dat  f00000335.dat
f00000032.dat  f00000108.dat  f00000184.dat  f00000260.dat  f00000336.dat
f00000033.dat  f00000109.dat  f00000185.dat  f00000261.dat  f00000337.dat
f00000034.dat  f00000110.dat  f00000186.dat  f00000262.dat  f00000338.dat
f00000035.dat  f00000111.dat  f00000187.dat  f00000263.dat  f00000339.dat
f00000036.dat  f00000112.dat  f00000188.dat  f00000264.dat  f00000340.dat
f00000037.dat  f00000113.dat  f00000189.dat  f00000265.dat  f00000341.dat
f00000038.dat  f00000114.dat  f00000190.dat  f00000266.dat  f00000342.dat
f00000039.dat  f00000115.dat  f00000191.dat  f00000267.dat  f00000343.dat
f00000040.dat  f00000116.dat  f00000192.dat  f00000268.dat  f00000344.dat
f00000041.dat  f00000117.dat  f00000193.dat  f00000269.dat  f00000345.dat
f00000042.dat  f00000118.dat  f00000194.dat  f00000270.dat  f00000346.dat
f00000043.dat  f00000119.dat  f00000195.dat  f00000271.dat  f00000347.dat
f00000044.dat  f00000120.dat  f00000196.dat  f00000272.dat  f00000348.dat
f00000045.dat  f00000121.dat  f00000197.dat  f00000273.dat  f00000349.dat
f00000046.dat  f00000122.dat  f00000198.dat  f00000274.dat  f00000350.dat
f00000047.dat  f00000123.dat  f00000199.dat  f00000275.dat  f00000351.dat
f00000048.dat  f00000124.dat  f00000200.dat  f00000276.dat  f00000352.dat
f00000049.dat  f00000125.dat  f00000201.dat  f00000277.dat  f00000353.dat
f00000050.dat  f00000126.dat  f00000202.dat  f00000278.dat  f00000354.dat
f00000051.dat  f00000127.dat  f00000203.dat  f00000279.dat  f00000355.dat
f00000052.dat  f00000128.dat  f00000204.dat  f00000280.dat  f00000356.dat
f00000053.dat  f00000129.dat  f00000205.dat  f00000281.dat  f00000357.dat
f00000054.dat  f00000130.dat  f00000206.dat  f00000282.dat  f00000358.dat
f00000055.dat  f00000131.dat  f00000207.dat  f00000283.dat  f00000359.dat
f00000056.dat  f00000132.dat  f00000208.dat  f00000284.dat  f00000360.dat
f00000057.dat  f00000133.dat  f00000209.dat  f00000285.dat  f00000361.dat
f00000058.dat  f00000134.dat  f00000210.dat  f00000286.dat  f00000362.dat
f00000059.dat  f00000135.dat  f00000211.dat  f00000287.dat  f00000363.dat
f00000060.dat  f00000136.dat  f00000212.dat  f00000288.dat  f00000364.dat
f00000061.dat  f00000137.dat  f00000213.dat  f00000289.dat  f00000365.dat
f00000062.dat  f00000138.dat  f00000214.dat  f00000290.dat  f00000366.dat
f00000063.dat  f00000139.dat  f00000215.dat  f00000291.dat  f00000367.dat
f00000064.dat  f00000140.dat  f00000216.dat  f00000292.dat  f00000368.dat
f00000065.dat  f00000141.dat  f00000217.dat  f00000293.dat  f00000369.dat
f00000066.dat  f00000142.dat  f00000218.dat  f00000294.dat  f00000370.dat
f00000067.dat  f00000143.dat  f00000219.dat  f00000295.dat  list.dat
f00000068.dat  f00000144.dat  f00000220.dat  f00000296.dat  w
f00000069.dat  f00000145.dat  f00000221.dat  f00000297.dat  x
f00000070.dat  f00000146.dat  f00000222.dat  f00000298.dat  y
f00000071.dat  f00000147.dat  f00000223.dat  f00000299.dat  z
f00000072.dat  f00000148.dat  f00000224.dat  f00000300.dat
f00000073.dat  f00000149.dat  f00000225.dat  f00000301.dat
f00000074.dat  f00000150.dat  f00000226.dat  f00000302.dat
f00000075.dat  f00000151.dat  f00000227.dat  f00000303.dat'

TEST 02 'large sizes'

	DO DATA sizes

	EXEC -w24x80 -C f*
		OUTPUT - $'f  ff  fff  ffff  fffff  ffffff  fffffff  ffffffff  fffffffff  ffffffffff'

	EXEC -w24x80 -C --testsize=1 -sk f*
		OUTPUT - $'    0 f       0 fff       0 fffff       0 fffffff       1 fffffffff
    0 ff      0 ffff      0 ffffff      0 ffffffff      1 ffffffffff'

	EXEC --testsize=32 -sk f*
		OUTPUT - $'8388608 f
12582912 ff
20971520 fff
37748736 ffff
71303168 fffff
138412032 ffffff
272629760 fffffff
541065216 ffffffff
1077936128 fffffffff
2151677952 ffffffffff'

	EXEC --format="%(size)22u %(name)s" f*
		OUTPUT - $'                     2 f
                     3 ff
                     5 fff
                     9 ffff
                    17 fffff
                    33 ffffff
                    65 fffffff
                   129 ffffffff
                   257 fffffffff
                   513 ffffffffff'

	EXEC --testsize=32 --format="%(blocks)18u %(size)22u %(name)s" f*
		OUTPUT - $'          16777216             8589934592 f
          25165824            12884901888 ff
          41943040            21474836480 fff
          75497472            38654705664 ffff
         142606336            73014444032 fffff
         276824064           141733920768 ffffff
         545259520           279172874240 fffffff
        1082130432           554050781184 ffffffff
        2155872256          1103806595072 fffffffff
        4303355904          2203318222848 ffffffffff'

	EXPORT	LC_NUMERIC=en

	EXEC --testsize=32 --format="%(blocks)'18u %(size)'22u %(name)s" f*
		OUTPUT - $'        16,777,216          8,589,934,592 f
        25,165,824         12,884,901,888 ff
        41,943,040         21,474,836,480 fff
        75,497,472         38,654,705,664 ffff
       142,606,336         73,014,444,032 fffff
       276,824,064        141,733,920,768 ffffff
       545,259,520        279,172,874,240 fffffff
     1,082,130,432        554,050,781,184 ffffffff
     2,155,872,256      1,103,806,595,072 fffffffff
     4,303,355,904      2,203,318,222,848 ffffffffff'
