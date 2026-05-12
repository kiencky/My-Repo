use qlthe
go

create table Lichsura
(
	STT int identity(1,1),
	ID nchar(10) ,
	Bienso nvarchar(20),
	DateTime smalldatetime,
	Phiguixe nvarchar(20)
)
go

insert into dbo.lichsura(id,bienso,DateTime,phiguixe) values (N'31AFDA11',N'29A4252',getdate(),N'5000');
insert into dbo.lichsura(id,bienso,DateTime,phiguixe) values (N'54VSAR12',N'29B3134',getdate(),N'5000');
go
--alter table lichsura add STT as row_number() over (order by id)
--drop table lichsura
go
--delete dbo.Lichsura
go