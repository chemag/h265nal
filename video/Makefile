

INFILE=~/work/video/raw/akiyo_cif.y4m
INFILEYUV=${INFILE}.yuv

RESYUV=352x288
FRAMESYUV=300

${INFILE}:
	wget https://media.xiph.org/video/derf/y4m/akiyo_cif.y4m

${INFILEYUV}: ${INFILE}
	ffmpeg -i ${INFILE} -f rawvideo -pix_fmt yuv420p ${INFILEYUV}

akiyo.kvazaar.qp_15.265: 
	ffmpeg -i ${INFILE} -c:v libkvazaar -kvazaar-params qp=15 $@

akiyo.kvazaar.qp_30.265: 
	ffmpeg -i ${INFILE} -c:v libkvazaar -kvazaar-params qp=30 $@

akiyo.kvazaar.qp_50.265: 
	ffmpeg -i ${INFILE} -c:v libkvazaar -kvazaar-params qp=50 $@

akiyo.x265.qp_15.265: 
	ffmpeg -i ${INFILE} -c:v libx265 -x265-params qp=15 $@

akiyo.x265.qp_30.265: 
	ffmpeg -i ${INFILE} -c:v libx265 -x265-params qp=30 $@

akiyo.x265.qp_50.265: 
	ffmpeg -i ${INFILE} -c:v libx265 -x265-params qp=50 $@


TURING=${HOME}/proj/turingcodec/build/release/turing/turing

akiyo.turing.qp_15.265: 
	${TURING} encode ${INFILEYUV} --frame-rate 30 --input-res ${RESYUV} --frames ${FRAMESYUV} --qp 15 -o $@

akiyo.turing.qp_30.265: 
	${TURING} encode ${INFILEYUV} --frame-rate 30 --input-res ${RESYUV} --frames ${FRAMESYUV} --qp 30 -o $@

akiyo.turing.qp_50.265: 
	${TURING} encode ${INFILEYUV} --frame-rate 30 --input-res ${RESYUV} --frames ${FRAMESYUV} --qp 50 -o $@


