
#!/bin/bash
echo
echo
echo -e  "testing P1 and echo $ ? at next line"
echo
echo
./a.out
echo $?

echo
echo
echo  -e "testing P2 and echo $ ? at next line"
echo
echo
./b.out
echo $?

echo
echo
echo -e  "testing P3 and echo $ ? at next line"
echo
echo

./c.out
echo $?

echo
echo
echo -e  "testing P4 and echo $ ? at next line"
echo
echo

./d.out
echo $?

echo
echo
echo  -e "testing P5 and echo $ ? at next line"
echo
echo

./e.out
echo $?

echo
echo
echo -e  "testing P6"
echo
echo

./f.out
echo $?

