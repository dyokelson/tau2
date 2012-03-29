/* remove whatever indexes and tables are there already, in reverse order */

DROP INDEX counter_value_index;
DROP INDEX counter_group_index;
DROP INDEX counter_trial_index;
DROP TABLE counter_value;
DROP TABLE counter_group;
DROP TABLE counter;

DROP INDEX measurement_value_index;
DROP INDEX measurement_group_index;
DROP INDEX measurement_trial_index;
DROP TABLE measurement_value;
DROP TABLE measurement_group;
DROP TABLE measurement_parameter;
DROP TABLE measurement;

DROP INDEX secondary_metadata_index;
DROP INDEX primary_metadata_index;
DROP TABLE secondary_metadata;
DROP TABLE primary_metadata;

DROP TABLE metric;
DROP TABLE thread;
DROP TABLE trial;
DROP TABLE data_source;

/* These are our supported parsers. */

CREATE TABLE data_source (
 id          INT UNIQUE NOT NULL,
 name        VARCHAR NOT NULL,
 description VARCHAR
);

INSERT INTO data_source (name,id,description) 
  VALUES ('ppk',0,'TAU Packed profiles (TAU)');
INSERT INTO data_source (name,id,description) 
  VALUES ('TAU profiles',1,'TAU profiles (TAU)');
INSERT INTO data_source (name,id,description) 
  VALUES ('DynaProf',2,'PAPI DynaProf profiles (UTK)');
INSERT INTO data_source (name,id,description) 
  VALUES ('mpiP',3,'mpiP: Lightweight, Scalable MPI Profiling (Vetter, Chambreau)');
INSERT INTO data_source (name,id,description) 
  VALUES ('HPM',4,'HPM Toolkit profiles (IBM)');
INSERT INTO data_source (name,id,description) 
  VALUES ('gprof',5,'gprof profiles (GNU)');
INSERT INTO data_source (name,id,description) 
  VALUES ('psrun',6,'PerfSuite psrun profiles (NCSA)');
INSERT INTO data_source (name,id,description) 
  VALUES ('pprof',7,'TAU pprof.dat output (TAU)');
INSERT INTO data_source (name,id,description) 
  VALUES ('Cube',8,'Cube data (FZJ)');
INSERT INTO data_source (name,id,description) 
  VALUES ('HPCToolkit',9,'HPC Toolkit profiles (Rice Univ.)');
INSERT INTO data_source (name,id,description) 
  VALUES ('SNAP',10,'TAU Snapshot profiles (TAU)');
INSERT INTO data_source (name,id,description) 
  VALUES ('OMPP',11,'OpenMP Profiler profiles (Fuerlinger)');
INSERT INTO data_source (name,id,description) 
  VALUES ('PERIXML',12,'Data Exchange Format (PERI)');
INSERT INTO data_source (name,id,description) 
  VALUES ('GPTL',13,'General Purpose Timing Library (ORNL)');
INSERT INTO data_source (name,id,description) 
  VALUES ('Paraver',14,'Paraver profiles (BSC)');
INSERT INTO data_source (name,id,description) 
  VALUES ('IPM',15,'Integrated Performance Monitoring (NERSC)');
INSERT INTO data_source (name,id,description) 
  VALUES ('Google',16,'Google profiles (Google)');
INSERT INTO data_source (name,id,description) 
  VALUES ('Cube3',17,'Cube 3D profiles (FZJ)');
INSERT INTO data_source (name,id,description) 
  VALUES ('Gyro',100,'Self-timing profiles from Gyro application');
INSERT INTO data_source (name,id,description) 
  VALUES ('GAMESS',101,'Self-timing profiles from GAMESS application');
INSERT INTO data_source (name,id,description) 
  VALUES ('Other',999,'Other profiles');

/* trials are the top level table */

CREATE TABLE trial (
 id                  SERIAL NOT NULL PRIMARY KEY,
 name                TEXT,
 collection_date     TIMESTAMP WITH TIME ZONE,
 data_source         INT,
 node_count          INT,
 contexts_per_node   INT,
 threads_per_context INT,
 FOREIGN KEY(data_source) REFERENCES data_source(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* threads make it convenient to identify measurement values.
   Special values for node_id:
   -1 mean (nulls ignored)
   -2 total
   -3 stddev (nulls ignored)
   -4 mean (nulls are 0 value)
   -5 stddev (nulls are 0 value)
   -6 min (nulls are ignored)
   -7 min (nulls are 0 value)
   -8 max
   -9 mode
*/

CREATE TABLE thread (
 id           SERIAL NOT NULL PRIMARY KEY,
 trial        INT NOT NULL,
 node_rank    INT NOT NULL,
 context_rank INT NOT NULL,
 thread_rank  INT NOT NULL,
 process_id   INT NOT NULL,
 thread_id    INT NOT NULL,
 FOREIGN KEY(trial) REFERENCES trial(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* primary metadata is metadata that is not nested, does not
   contain unique data for each thread. */

CREATE TABLE primary_metadata (
 trial    INT NOT NULL,
 name     VARCHAR NOT NULL,
 value    TEXT NOT NULL,
 FOREIGN KEY(trial) REFERENCES trial(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* create an index for faster queries against the primary_metadata table */
CREATE INDEX primary_metadata_index on primary_metadata (trial, name);

/* primary metadata is metadata that could be nested, could
   contain unique data for each thread, and could be an array. */

CREATE TABLE secondary_metadata (
 id       SERIAL NOT NULL PRIMARY KEY,
 trial    INT NOT NULL,
 thread   INT,
 parent   INT,
 name     VARCHAR NOT NULL,
 value    TEXT,
 is_array BOOLEAN DEFAULT FALSE,
 FOREIGN KEY(trial) REFERENCES trial(id) ON DELETE NO ACTION ON UPDATE NO ACTION,
 FOREIGN KEY(thread) REFERENCES thread(id) ON DELETE NO ACTION ON UPDATE NO ACTION,
 FOREIGN KEY(parent) REFERENCES secondary_metadata(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* create an index for faster queries against the secondary_metadata table */
CREATE INDEX secondary_metadata_index on secondary_metadata (trial, name, thread, parent);

/* metrics are things like num_calls, num_subroutines, TIME, PAPI
   counters, and derived metrics. */

CREATE TABLE metric (
 id      SERIAL NOT NULL PRIMARY KEY,
 name    TEXT NOT NULL,
 trial   INT NOT NULL,
 derived BOOLEAN NOT NULL DEFAULT FALSE,
 FOREIGN KEY(trial) REFERENCES trial(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* measurements are timers, capturing some interval value.  For callpath or
   phase profiles, the parent refers to the calling function or phase. */

CREATE TABLE measurement (
 id                SERIAL NOT NULL PRIMARY KEY,
 trial             INT NOT NULL,
 name              TEXT NOT NULL,
 source_file       TEXT,
 line_number       INT,
 line_number_end   INT,
 column_number     INT,
 column_number_end INT,
 parent            INT,
 FOREIGN KEY(trial) REFERENCES trial(id) ON DELETE NO ACTION ON UPDATE NO ACTION,
 FOREIGN KEY(parent) REFERENCES measurement(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* measurement index on the trial and name columns */
CREATE INDEX measurement_trial_index on measurement (trial, name);

/* measurements groups are the groups such as TAU_DEFAULT,
   MPI, OPENMP, TAU_PHASE, TAU_CALLPATH, TAU_PARAM, etc. 
   This mapping table allows for NxN mappings between timers
   and groups */

CREATE TABLE measurement_group (
 measurement INT,
 group_name  VARCHAR NOT NULL,
 FOREIGN KEY(measurement) REFERENCES measurement(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* index for faster queries into groups */
CREATE INDEX measurement_group_index on measurement_group (measurement, group_name);

/* measurement parameters are parameter based profile values. 
   an example is foo (x,y) where x=4 and y=10. In that example,
   measurement would be the index of the measurement with the
   name 'foo (x,y) <x>=<4> <y>=<10>'. This table would have two
   entries, one for the x value and one for the y value.
*/

CREATE TABLE measurement_parameter (
 measurement     INT,
 parameter_name  VARCHAR NOT NULL,
 parameter_value VARCHAR NOT NULL,
 FOREIGN KEY(measurement) REFERENCES measurement(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* counters measure some counted value. */

CREATE TABLE counter (
 id          SERIAL      NOT NULL PRIMARY KEY,
 trial       INT         NOT NULL,
 name        TEXT        NOT NULL,
 source_file TEXT,
 line_number INT,
 parent      INT,
 FOREIGN KEY(trial) REFERENCES trial(id) ON DELETE NO ACTION ON UPDATE NO ACTION,
 FOREIGN KEY(parent) REFERENCES measurement(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* counter index on the trial and name columns */
CREATE INDEX counter_trial_index on counter (trial, name);

/* counter groups are the groups of counters. This table
   allows for NxN mappings of counters to groups. */

CREATE TABLE counter_group (
 counter    INT,
 group_name VARCHAR NOT NULL,
 FOREIGN KEY(counter) REFERENCES counter(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* index for faster queries into groups */
CREATE INDEX counter_group_index on counter_group (counter, group_name);

/* measurement values have the measurement of one timer
   on one thread for one metric. */

CREATE TABLE measurement_value (
 measurement           INT NOT NULL,
 thread                INT NOT NULL,
 metric                INT NOT NULL,
 inclusive_value       DOUBLE PRECISION,
 exclusive_value       DOUBLE PRECISION,
 sum_exclusive_squared DOUBLE PRECISION,
 time                  DOUBLE PRECISION,
 FOREIGN KEY(measurement) REFERENCES measurement(id) ON DELETE NO ACTION ON UPDATE NO ACTION,
 FOREIGN KEY(thread) REFERENCES thread(id) ON DELETE NO ACTION ON UPDATE NO ACTION,
 FOREIGN KEY(metric) REFERENCES metric(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* one metric, one thread, one timer */
CREATE INDEX measurement_value_index on measurement_value (measurement, metric, thread);

CREATE TABLE counter_value (
    counter            INT NOT NULL,
    thread             INT NOT NULL,
    sample_count       INT,         
    maximum_value      DOUBLE PRECISION,
    minimum_value      DOUBLE PRECISION,
    mean_value         DOUBLE PRECISION,
    standard_deviation DOUBLE PRECISION,
    FOREIGN KEY(counter) REFERENCES counter(id) ON DELETE NO ACTION ON UPDATE NO ACTION,
    FOREIGN KEY(thread) REFERENCES thread(id) ON DELETE NO ACTION ON UPDATE NO ACTION
);

/* one thread, one counter */
CREATE INDEX counter_value_index on counter_value (counter, thread);


