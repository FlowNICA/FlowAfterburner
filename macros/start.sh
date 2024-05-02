#!/bin/bash

#
#SBATCH -D /scratch1/parfenov/TMP
#SBATCH -J QATools
#SBATCH --mem-per-cpu=2G
#SBATCH -p nica
#SBATCH --time=3:30:00
#SBATCH -a 1-1
#
#SBATCH -o /scratch1/parfenov/TMP/slurm_simAB_%A_%a.out
#SBATCH -e /scratch1/parfenov/TMP/slurm_simAB_%A_%a.out
#

export SKIP_LINES=${1}

source /cvmfs/nica.jinr.ru/sw/os/login.sh latest
module add mpddev/v23.09.23-1

export JOB_ID=$SLURM_ARRAY_JOB_ID
export TASK_ID=$(( SLURM_ARRAY_TASK_ID + SKIP_LINES ))

#Main directory
export MAIN_DIR=/lhep/users/parfenov/Soft/FlowAfterburner

export INPUTLIST=${MAIN_DIR}/macros/dcm_xecs_3.02gev.list

export INPUT=`sed "${TASK_ID}q;d" ${INPUTLIST}`
export SHORTNAME=`basename $INPUT`
export OUT_DIR=`dirname ${INPUT} | sed -e "s|files|flow|g"`
export OUTPUT=${OUT_DIR}/${SHORTNAME}
export OUTPUT1=`echo $OUTPUT | sed -e "s|mcini|mcpico|g"`
export OUT_DIR1=`dirname $OUTPUT1`

export MCINI_DIR=/scratch1/parfenov/Soft/mcini/
source ${MCINI_DIR}/macro/config.sh

export AFTERBURNER=${MAIN_DIR}/redoFlow.C
export FLOW_FILE=${MAIN_DIR}/qa_jam_auau_enscan2_3.0gev_MD3.root
export P2N_RATIO=1.1
export CONVERTER=${MAIN_DIR}/convert.C

mkdir -p $OUT_DIR
mkdir -p $OUT_DIR1

echo "File no.    : ${TASK_ID}"
echo "Input file  : ${INPUT}"
echo "Output file : ${OUTPUT}"
echo "Output file : ${OUTPUT1}"
echo "Flow file   : ${FLOW_FILE}"
echo "p/n vn ratio: ${P2N_RATIO}"

time root -b -q -l $AFTERBURNER'("'$INPUT'","'$OUTPUT'","'$FLOW_FILE'",'$P2N_RATIO')'

time root -l -b -q $CONVERTER'("'$OUTPUT'","'$OUTPUT1'")'

echo "Job is done!"
