create table Schema (
	version     text not null,
	description text unique not null,
	flags       text
);

create table Namespaces (
	name    text not null UNIQUE primary key
);

create table 'QualifierTypes' (
	'name'      text not null,
	'nsname'    text not null,
	'data'      blob not null,
   primary key (name, nsname)
);

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
	depth        integer NOT null
);

create index SuperClassesIdx on SuperClasses (subcid, supercid, depth);


