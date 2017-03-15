<?php

include "database/database.php";

    class database extends mysqli
    {
        private $host = "localhost";
        private $user = "DEWI";
        private $password = "dewiPass";
        private $database = "DEWI_database";

        private $db;

        function __construct()
        {
            $this->connectDB();
            if($this->connect_error)
            {


            }

            $this->closeDB();
        }
        function getDatabaseName(){return $this->database;}
        function getHostName(){return $this->host;}
        function getUserName(){return $this->user;}
        function getPassword(){return $this->password;}

        function connectDB()
        {
            $this->connect($this->host,$this->user,$this->password,$this->database);
            if($this->connect_error)
                echo("No database connection: ".$this->connect_error);

        }
        function closeDB()
        {
            $this->close();
        }
    }

?>