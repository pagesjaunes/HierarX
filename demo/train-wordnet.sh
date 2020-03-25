export ENTITY=$1

export EXPDIR=${PWD}/experiment-${ENTITY}
mkdir -p ${EXPDIR}



python -m pyhierarx show \
  ${EXPDIR} \
  ${EXPDIR}/final-state.html \
  --entities=${EXPDIR}/${ENTITY}-entities.csv \
  --edges=${EXPDIR}/${ENTITY}-edges.csv \
  --animation=True \
  --nframes=40 \
  --step=2 \
  --interactive=True