export ENTITY=$1

export EXPDIR=${PWD}/experiment-${ENTITY}
mkdir -p ${EXPDIR}

python -m pyhierarx wordnet-exp "${ENTITY}" ${EXPDIR}

${HIERARX_BIN} \
  -dim=2 \
  -nvoc=2000 \
  -thread=1 \
  -expdir=${EXPDIR} \
  -input=${EXPDIR}/${ENTITY}-similarities.hierarx \
  -lr=0.001 \
  -hmode=Poincare \
  -sampling=20 \
  -bs=10 \
  -momentum=0.5 \
  -niter=1000000 \
  -posthres=0 \
  -maxposthres=0.1 \
  -plateau=0 \
  -checkpoint=5000 \
  --similarity \
  --nesterov \
  --symmetric \
  --movie \
  --declr \
  -minlr=0.0001 \
  -c=0.01 \
  --lorentzian

python -m pyhierarx show \
  ${EXPDIR}/embedding.bin \
  ${EXPDIR}/final-state.pdf \
  --entities=${EXPDIR}/${ENTITY}-entities.csv \
  --edges=${EXPDIR}/${ENTITY}-edges.csv \
  --interactive=False

python -m pyhierarx show \
  ${EXPDIR} \
  ${EXPDIR}/final-state.html \
  --entities=${EXPDIR}/${ENTITY}-entities.csv \
  --edges=${EXPDIR}/${ENTITY}-edges.csv \
  --animation=True \
  --nframes=200 \
  --step=2 \
  --interactive=True