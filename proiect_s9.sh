if [ $# -ne 1 ] 
	then echo "bash script <c>"
	exit 1
fi

if [[ "$1" =~ [^a-zA-Z0-9] ]] 
	then echo "Caracterul nu e alfanumeric"
fi

character=$1
count=0
regex1='^[A-Z][a-zA-Z0-9, !?]+[.!?]$'

while read -r line; 
do
   if [[ $line =~ $regex1 ]] && ! [[ $line =~ ,\ și ]]
	then  
		echo "Propoziția respectă condițiile."
		if [[ $line == *"$character"* ]] 
		then ((count++))
		fi
	else
		 echo "Propoziția nu respectă condițiile."
	fi
		
    
done


echo $count

#
# O propoziție se considera corecta dacă respectă următoarele condiții:
# - începe cu litera mare
# - conține doar litere mari, mici, cifre, spații, virgula, punct semnul exclamarii sau semnul întrebării
# - se termina cu semnul întrebării, semnul exclamarii sau punct
# - nu contine virgula (,) înainte de și

# 