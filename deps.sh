# Mongoose - This is a simple dep line generator
# modified by xythobuz
#
# Call it passing CFLAGS and a source filename
# to generate a dep line

cc -MM -w $@
OUT=$?
if [ $OUT -eq 0 ];then
    printf "\$(BUILDDIR)/"
    cc -MM -w $@
    printf "\t \$(DO_CC)\n\n"
fi
