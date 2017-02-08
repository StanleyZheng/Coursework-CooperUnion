#!
echo "---------------"
ls
ls -al >output
cat output
echo "---------------"
cat output >>output2
cat output2
cat <output >>output2
cat output2
echo "---------------"
ls -l tempnothere 2>outerror
cat outerror
ls -l tempnotactuallyhereagain 2>>outerror
cat outerror
ls -l asdfasdf 2>>outerror
cat outerror
