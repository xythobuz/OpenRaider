
echo "<html><head><meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=iso-8859-1\"><meta name=\"generator\" content=\"sshot.sh by Mongoose\"></head><title>OpenRaider: Lastest Screenshot</title><body bgcolor=\"#6C6C80\" text=\"#0F0F0F\" link=\"#3F3F3F\" vlink=\"#2F2F2F\" alink=\"#6C6C80\"><center><img src=\"images/screenshots/$1\"><br><br>$2<br><a href=\"images/screenshots/\">More screenies</a><br><a href=\"index.php\">Back to OpenRaider</a></center></body></html>" > /tmp/sshot.html

scp $1 /tmp/sshot.html orbital.sf.net:~/openraider/htdocs/images/screenshots/
