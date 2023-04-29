CREATE TABLE `tablename`(
`sendtimr` varchar(30) NOT NULL,
`id` int(6) NOT NULL,
`temperature` varchar(10) NOT NULL,
`humidity` varchar(10) NOT NULL,
`dewpoint` varchar(10) NOT NULL,
`pressure` varchar(10) NOT NULL,
`samplemins` varchar(10) NOT NULL,
`comment` varchar(100) NOT NULL )
ENGINE=InnoDB DEFAULT CHARSET=latin1;
ALTER TABLE`tablename` ADD PRIMARY KEY(`id`);