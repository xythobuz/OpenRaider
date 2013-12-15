# Mongoose - This is a simple dep line generator
#
# Call it passing CFLAGS and a source filename
# to generate a dep line

cc -MM $@ > /dev/null
OUT=$?
if [ $OUT -eq 0 ];then
    printf "\$(BUILDDIR)/"
    cc -MM $@
    printf "\t \$(DO_CC)\n\n"
fi
