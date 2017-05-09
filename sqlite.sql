/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 * Author:  ever
 * Created: 2017-4-14
 */

create table if not exists t_tgn(
    tgn varchar(64),
    c_type tinyint,
    c_quality tinyint,
    begin_tm BIGINT,
    block_id bigint,
    start_off bigint,
    cache_len int,
    elem_size int --#个数
);
create table if not exists t_tgn(
    tgn varchar(64),
    block_id int,
    start_off int
);
/*
    
*/