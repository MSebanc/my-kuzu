create node table tableOfTypes (id INT64, int64Column INT64, doubleColumn DOUBLE, booleanColumn BOOLEAN, dateColumn DATE, timestampColumn TIMESTAMP, stringColumn STRING, listOfInt64 INT64[], listOfString STRING[], listOfListOfInt64 INT64[][], fixedSizeList INT64[3], listOfFixedSizeList INT64[3][], structColumn STRUCT(ID int64, name STRING), PRIMARY KEY (id));
