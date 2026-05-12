use qlthe
go

create table Danhsachthe
(
	STT int identity(1,1) ,
	ID nchar(10) ,
	Ngaythem nvarchar(20),
	primary key (STT,ID)
)
go

insert into dbo.danhsachthe(id,ngaythem) values (N'054AAE87',CONVERT(nvarchar(10), GETDATE(), 103));
insert into dbo.danhsachthe(id,ngaythem) values (N'11EDAD87',CONVERT(nvarchar(10), GETDATE(), 103));
--alter table danhsachthe add STT as row_number() over (order by id)

--drop table danhsachthe