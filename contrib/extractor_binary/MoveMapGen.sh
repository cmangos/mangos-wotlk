#!/bin/sh

# This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

## Syntax of this helper
## First param must be number of to be used CPUs (only 1, 2, 4, 8 supported) or "offmesh" to recreate the special tiles from the OFFMESH_FILE
## Second param can be an additional filename for storing log
## Third param can be an addition filename for storing detailed log

## Additional Parameters to be forwarded to MoveMapGen, see mmaps/readme for instructions
PARAMS="--silent"

## Already a few map extracted, and don't care anymore
EXCLUDE_MAPS=""
#EXCLUDE_MAPS="0 1 530 571" # example to exclude the continents

## Offmesh file
OFFMESH_FILE="offmesh.txt"

## Normal log file (if not overwritten by second param
LOG_FILE="MoveMapGen.log"
## Detailed log file
DETAIL_LOG_FILE="MoveMapGen_detailed.log"

## ! Use below only for finetuning or if you know what you are doing !

## All maps
LIST_A="571"
LIST_B="1"
LIST_C="530"
LIST_D="0"
LIST_E="169 531 409 545 604 542 44 109 389 565 450 617 616 592 582 489"
LIST_F="603 631 574 560 269 602 309 585 566 469 25 615 369 230 649 576 547 555 48 35 129 558 557 249 449 588 586 608"
LIST_G="607 533 509 543 30 600 532 650 553 43 554 34"
LIST_H="619 628 723 609 564 37 534 595 209 568 529 573 658 47 349 229 546 189 552 540 598 556 601 544 593 42 451 587"
LIST_I="591 548 33"
LIST_J="594 572 562 329"
LIST_K="596 580 429 36 13"
LIST_L="289 724 559 599 668 90 550 70 632 624 618 590 584 589 578 575"

badParam()
{
 echo "ERROR! Bad arguments!"
 echo "You can (re)extract mmaps with this helper script,"
 echo "or recreate only the tiles from the offmash file"
 echo
 echo "Call with number of processes (1,2,4,8) to create mmaps"
 echo "Call with 'offmesh' to reextract the tiles from offmash file"
 echo "Example: ./MoveMapGen.sh 8"
 echo
 echo "For further fine-tuning edit this helper script"
 echo
}

if [ "$#" = "3" ]
then
 LOG_FILE=$2
 DETAIL_LOG_FILE=$3
elif [ "$#" = "2" ]
then
 LOG_FILE=$2
fi

# Offmesh file provided?
OFFMESH=""
if [ "$OFFMESH_FILE" != "" ]
then
 if [ ! -f "$OFFMESH_FILE" ]
 then
   echo "ERROR! Offmesh file $OFFMESH_FILE could not be found."
   echo "Provide valid file or none. You need to edit the script"
   exit 1
 else
   OFFMESH="--offMeshInput $OFFMESH_FILE"
 fi
fi

# Function to process a list
createMMaps()
{
 for i in $@
 do
   for j in $EXCLUDE_MAPS
   do
     if [ "$i" = "$j" ]
     then
       continue 2
     fi
   done
   ./MoveMapGen $PARAMS $OFFMESH $i | tee -a $DETAIL_LOG_FILE
   echo "`date`: (Re)created map $i" | tee -a $LOG_FILE
 done
}

createHeader()
{
 echo "`date`: Start creating MoveMaps" | tee -a $LOG_FILE
 echo "Used params: $PARAMS $OFFMESH" | tee -a $LOG_FILE
 echo "Detailed log can be found in $DETAIL_LOG_FILE" | tee -a $LOG_FILE
 echo "Start creating MoveMaps" | tee -a $DETAIL_LOG_FILE
 echo
 echo "Be PATIENT - This will take a long time and might also have gaps between visible changes on the console."
 echo "WAIT until you are informed that 'creating MoveMaps' is 'finished'!"
}

# Create mmaps directory if not exist
if [ ! -d mmaps ]
then
 mkdir mmaps
fi

# Param control
case "$1" in
 "1" )
   createHeader $1
   createMMaps $LIST_A $LIST_B $LIST_C $LIST_D $LIST_E $LIST_F $LIST_G $LIST_H $LIST_I $LIST_J $LIST_K $LIST_L &
   ;;
 "2" )
   createHeader $1
   createMMaps $LIST_A $LIST_C $LIST_E $LIST_G $LIST_J $LIST_L &
   createMMaps $LIST_B $LIST_D $LIST_F $LIST_H $LIST_I $LIST_K &
   ;;
 "4" )
   createHeader $1
   createMMaps $LIST_A $LIST_E &
   createMMaps $LIST_B $LIST_F $LIST_I &
   createMMaps $LIST_C $LIST_G $LIST_J $LIST_L &
   createMMaps $LIST_D $LIST_H $LIST_K &
   ;;
 "8" )
   createHeader $1
   createMMaps $LIST_A &
   createMMaps $LIST_B &
   createMMaps $LIST_C &
   createMMaps $LIST_D &
   createMMaps $LIST_E $LIST_I $LIST_J $LIST_K &
   createMMaps $LIST_F &
   createMMaps $LIST_G $LIST_L &
   createMMaps $LIST_H &
   ;;
 "offmesh" )
   echo "`date`: Recreate offmeshs from file $OFFMESH_FILE" | tee -a $LOG_FILE
   echo "Recreate offmeshs from file $OFFMESH_FILE" | tee -a $DETAIL_LOG_FILE
   while read map tile line
   do
     ./MoveMapGen $PARAMS $OFFMESH $map --tile $tile | tee -a $DETAIL_LOG_FILE
     echo "`date`: Recreated $map $tile from $OFFMESH_FILE" | tee -a $LOG_FILE
   done < $OFFMESH_FILE &
   ;;
 * )
   badParam
   exit 1
   ;;
esac

wait

echo  | tee -a $LOG_FILE
echo  | tee -a $DETAIL_LOG_FILE
echo "`date`: Finished creating MoveMaps" | tee -a $LOG_FILE
echo "`date`: Finished creating MoveMaps" >> $DETAIL_LOG_FILE
