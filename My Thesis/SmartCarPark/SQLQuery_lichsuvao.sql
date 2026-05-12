use qlthe
go

create table Lichsuvao
(
	STT int identity(1,1),
	ID nchar(10) ,
	Bienso nvarchar(20),
	DateTime smalldatetime
)
go

insert into dbo.lichsuvao(id,bienso,DateTime) values (N'31AFDA11',N'29A4252',getdate());
insert into dbo.lichsuvao(id,bienso,DateTime) values (N'54VSAR12',N'29B3134',getdate());
go
--alter table lichsuvao add STT as row_number() over (order by id)
--drop table lichsuvao
go