INSERT INTO 'Schema' VALUES ('1.0.0', 'what is the description for?', NULL);

INSERT INTO Namespaces VALUES ('root');
INSERT INTO Namespaces VALUES ('root/cimv2');

INSERT INTO QualifierTypes VALUES ('TestQualifier1', 'root', 'TestQualifier1 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier2', 'root', 'TestQualifier2 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier3', 'root', 'TestQualifier3 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier4', 'root', 'TestQualifier4 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier5', 'root', 'TestQualifier5 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier6', 'root', 'TestQualifier6 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier7', 'root', 'TestQualifier7 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier8', 'root', 'TestQualifier8 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier9', 'root/cimv2', 'TestQualifier9 data -- this could be a bunch of xml or binary junk');
INSERT INTO QualifierTypes VALUES ('TestQualifier10', 'root/cimv2', 'TestQualifier10 data -- this could be a bunch of xml or binary junk');

INSERT INTO Classes (name, nsname, data) VALUES ('TestClass1', 'root', 'TestClass1 data -- this could be a bunch of xml or binary junk. This class doesn''t have any superclass');
INSERT INTO Classes (name, nsname, data) VALUES ('TestClass2', 'root', 'TestClass2 data -- This class is derived from TestClass1');
INSERT INTO SuperClasses
SELECT last_insert_rowid(), supercid, depth+1
  FROM Classes as c, SuperClasses as sc
  WHERE c.name = 'TestClass1' and sc.subcid = c.cid and nsname = 'root'
UNION ALL
SELECT last_insert_rowid(), cid, 1
  FROM Classes
  WHERE name = 'TestClass1' and nsname = 'root';

INSERT INTO Classes (name, nsname, data) VALUES ('TestClass3', 'root', 'TestClass3 data -- This class is derived from TestClass2');
INSERT INTO SuperClasses
SELECT last_insert_rowid(), supercid, depth+1
  FROM Classes as c, SuperClasses as sc
  WHERE c.name = 'TestClass2' and sc.subcid = c.cid and nsname = 'root'
UNION ALL
SELECT last_insert_rowid(), cid, 1
  FROM Classes
  WHERE name = 'TestClass2' and nsname = 'root';

INSERT INTO Classes (name, nsname, data) VALUES ('TestClass4', 'root', 'TestClass4 data -- This class is derived from TestClass3');
INSERT INTO SuperClasses
SELECT last_insert_rowid(), supercid, depth+1
  FROM Classes as c, SuperClasses as sc
  WHERE c.name = 'TestClass3' and sc.subcid = c.cid and nsname = 'root'
UNION ALL
SELECT last_insert_rowid(), cid, 1
  FROM Classes
  WHERE name = 'TestClass3' and nsname = 'root';

INSERT INTO Classes (name, nsname, data) VALUES ('TestClass5', 'root', 'TestClass5 data -- This class is derived from TestClass3');
INSERT INTO SuperClasses
SELECT last_insert_rowid(), supercid, depth+1
  FROM Classes as c, SuperClasses as sc
  WHERE c.name = 'TestClass3' and sc.subcid = c.cid and nsname = 'root'
UNION ALL
SELECT last_insert_rowid(), cid, 1
  FROM Classes
  WHERE name = 'TestClass3' and nsname = 'root';

INSERT INTO Classes (name, nsname, data) VALUES ('TestClass6', 'root', 'TestClass6 data -- This class is derived from TestClass3');
INSERT INTO SuperClasses
SELECT last_insert_rowid(), supercid, depth+1
  FROM Classes as c, SuperClasses as sc
  WHERE c.name = 'TestClass3' and sc.subcid = c.cid and nsname = 'root'
UNION ALL
SELECT last_insert_rowid(), cid, 1
  FROM Classes
  WHERE name = 'TestClass3' and nsname = 'root';

INSERT INTO Classes (name, nsname, data) VALUES ('TestClass7', 'root', 'TestClass7 data -- This class is derived from TestClass2');
INSERT INTO SuperClasses
SELECT last_insert_rowid(), supercid, depth+1
  FROM Classes as c, SuperClasses as sc
  WHERE c.name = 'TestClass2' and sc.subcid = c.cid and nsname = 'root'
UNION ALL
SELECT last_insert_rowid(), cid, 1
  FROM Classes
  WHERE name = 'TestClass2' and nsname = 'root';

INSERT INTO Classes (name, nsname, data) VALUES ('TestClass8', 'root', 'TestClass8 data -- This class is derived from TestClass1');
INSERT INTO SuperClasses
SELECT last_insert_rowid(), supercid, depth+1
  FROM Classes as c, SuperClasses as sc
  WHERE c.name = 'TestClass1' and sc.subcid = c.cid and nsname = 'root'
UNION ALL
SELECT last_insert_rowid(), cid, 1
  FROM Classes
  WHERE name = 'TestClass1' and nsname = 'root';







