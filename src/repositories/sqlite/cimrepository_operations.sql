select count(*)
from Namespaces
where name = 'root';

select count(*)
from Namespaces
where name = 'test';

insert into Namespaces (name) values ('test');

select count(*)
from Namespaces
where name = 'test';

select name from Namespaces;

delete from QualifierTypes
where nsname = 'test';

delete from Namespaces
where name = 'test';
	
select name from Namespaces;


---  Qualifier Types  ---
-- GetQualifier:
select data
from QualifierTypes
where nsname = 'root' and name = <Qual Name>;

-- CreateQualifier (doesn't really exist, but SetQualifier is a transaction of GetQualifier than either CreateQualifier or ModifyQualifier):
insert into QualifierTypes
values(<Qual Name>, <Namespace>, <Qual Data>);

ModifyQualifier:
	update QualifierTypes
	set data=<New Qual Data>
	where nsname = <Namespace> and name = <Qual Name>;

DeleteQualifier:
	delete from QualifierTypes
	where nsname = <Namespace> and name = <Qual Name>;
	
EnumerateQualifiers:
	select data
	from QualifierTypes as qt
	where nsname = <Namespace>;
	

---  Classes  ---
create table Classes (
	cid         integer primary key,
	name        text not null,
	nsname      text not null,
	data        blob not null
);

create index ClassesIdx on Classes (name, nsname);

create table SuperClasses (
	subcid       integer not null,
	supercid     integer not null,
	depth        integer non null
);

create index SuperClassesIdx on SuperClasses (subcid, supercid, depth);

GetClass:
	select data
	from Classes
	where nsname = <Namespace> and name = <Class Name>;
	
DeleteClass:
	get all subclasses to recursively delete using EnumerateClassNames - maybe be a bit more efficient?
	
	delete from SuperClasses
	where subcid = (select cid from Classes where nsname = <Namespace> and name = <Class Name>);
	
	delete from Classes
	where nsname = <Namespace> and name = <Class Name>;
	
CreateClass:
	insert into Classes	(name, nsname, data)
	values(<Class Name>, <Namespace>, <Class Data>);
	
  If the class has a superclass:
	insert into SuperClasses
	select last_insert_rowid(), supercid, depth+1
	  from Classes as c, SuperClasses as sc
	  where c.name = <SuperClass Name> and SuperClasses.subcid = c.cid and nsname = <Namespace>
	union all
	select last_insert_rowid(), cid, 1
	  from Classes
	  where name = <SuperClass Name> and nsname = <Namespace>;


ModifyClass:
	update Classes
	set data=<New Class Data>
	where nsname = <Namespace> and name = <Class Name>;

EnumerateClasses - ClassName=NULL, DeepInheritance=true - i.e. all classes:
	select data
	from Classes
	where nsname = <Namespace>;
	
EnumerateClasses - ClassName=NULL, DeepInheritance=false - i.e. all root classes:
	select data
	from Classes
	where nsname = <Namespace> and cid not in (select subcid from SuperClasses);
	
EnumerateClasses - ClassName=<name>, DeepInheritance=true - i.e. all classes derived from ClassName:
	select c.data
	from Classes as c, SuperClasses as sc
	where c.cid = sc.subcid and sc.supercid in
	(select cid from Classes where nsname = <Namespace> and name = <ClassName>);
	
EnumerateClasses - ClassName=<name>, DeepInheritance=false - i.e. classes directly derived from ClassName:
	select c.data
	from Classes as c, SuperClasses as sc
	where c.cid = sc.subcid and sc.supercid in
	(select cid from Classes where nsname = <Namespace> and name = <ClassName>)
	and sc.depth = 1
	
EnumerateClassNames - ClassName=NULL, DeepInheritance=true - i.e. all classes:
	select name
	from Classes
	where nsname = <Namespace>;
	
EnumerateClassNames - ClassName=NULL, DeepInheritance=false - i.e. all root classes:
	select name
	from Classes
	where nsname = <Namespace> and cid not in (select subcid from SuperClasses);
	
EnumerateClassNames - ClassName=<name>, DeepInheritance=true - i.e. all classes derived from ClassName:
	select c.name
	from Classes as c, SuperClasses as sc
	where c.cid = sc.subcid and sc.supercid in
	(select cid from Classes where nsname = <Namespace> and name = <ClassName>);
	
EnumerateClassNames - ClassName=<name>, DeepInheritance=false - i.e. classes directly derived from ClassName:
	select c.name
	from Classes as c, SuperClasses as sc
	where c.cid = sc.subcid and sc.supercid in
	(select cid from Classes where nsname = <Namespace> and name = <ClassName>)
	and sc.depth = 1
	


