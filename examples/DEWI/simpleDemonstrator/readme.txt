
//to compile 
 make TARGET=zoul 
//or if you don't like TARGET
make TARGET=zoul savetarget 
//then you can only put
make

//after for uploading,    0 for slave   and 1 for master,  you can also compile directly with this, without put previous command (be carefull with target)

sudo make simpleDemo.upload DEFINES+=MASTERNODE=1


//finally you can see in the terminal, if you want, with

sudo make login



