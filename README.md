# iGD: Reshape and integrate large-scale data sources for highly efficient genome analysis

## Summary
Genome analysis usually requires comparing one set of genomic loci (region set) to many other annotated region sets that may come from one or more large-scale data sources. A wide variety of computational tools, such as LOLA, BART, Bedtools, Tabix, GenomeRunner and Giggle, have been developed for this purpose. Among these tools, Giggle claims to be over three orders of magnitude faster than others in searching millions to billions of genomic intervals. **iGD**, an integrated genome database, takes a database approach that directly integrates large-scale data sources into an easily searchable database by reshaping the data records. As a database, iGD takes not only the genomic regions, but also the signal levels and/or strand information, which makes parameterized query (dynamic search) an easy task. The search speed of iGD is one to two orders of magnitude faster than Giggle while the database size is several times smaller than that of the Giggle index files. 


## Introduction
Genome data sources, such as Roadmap, UCSC, and Cistrome, contain thousand of annotated data sets and each dataset has thousands to millions of genomic regions. 
The enrichment analysis is to find overlaps between regions of all data sets in the data source and all regions in the query data set. This becomes challenging when the size of the data source regions and the size of the query regions are large. For example, UCSC human data source contains ~7 billion regions or intervals, so if the query set has 100,000 intervals, then a total of 700 trillion comparisons may be involved. To effectively search for the overlaps, different indexing techniques have been developed. For example, Giggle employs a B+ tree to build many pure indexing files, and then a query is carried out on these indexing files instead of on the original data. Using the indexing, the actual search space is significantly reduced.
 
The goal of iGD is to build a database that integrates all of the genomic data sets from one or more data sources and minimizes the actual search space for a general query. To achieve this, iGD reshapes the data sources by dividing the genome into a large number of equal-size bins (~200,000 bins) and arranging data records of the data sources into the bin or bins they intersect. Data in each bin will be saved as a block of a single file. The file head contains the location of the bins and the number of iGD data elements in each bin. Each iGD data element contains the index of the original dataset, the original genomic region (start and end coordinates), and a value (signal level and/or strand information). To find overlaps of a query, one only needs to load one or a few bin block data instead of all data sets, which minimizes the data loading time and the comparisons are carried out only in the loaded bins, which minimizes the actual search space. A link with details about the implementation will be provided later. 
 

## How to build iGD

If zlib is not already installed, install it:
```
sudo apt-get install libpng12-0
```
Then:
```
git clone https://github.com/databio/iGD.git
cd iGD
make
```
the executable `igd` is in the subfolder `bin`. And then copy it to /usr/local/bin.

## How to run iGD

### 1. Create iGD database from a genome data source
 
```
igd create "/path/to/data_source_folder/*" "/path/to/igd_folder/" "databaseName" [option]

where:

- "path/to/data_source_folder/" is the path of the folder that contains .bed.gz or .bed data files.

- "path/to/igd_folder/" is the path to the output igd folder;

- "databaseName" is the name you give to the database, for eaxmple, "roadmap"

option:

-b: bin-size (power of 2; default 14, which is 16384 bp)
```

### 2. Search iGD for overlaps
```
igd search "path/to/query_file" "path/to/igd_data_file" [options]

where:

- path/to/query_file is the path to the query file

- path/to/igd_data_file is the path to the igd data (mode 1)

options:

-v: dynamic search with threshold signal value (0-1000)

-o: output file-name
```

For a detailed example, please check out the `vignettes`.

## iGD databases
Downloads of some iGD databases (fully created and directly searchable by using iGD) are available at [big.databio.org/example_data/igd](http://big.databio.org/example_data/igd).

## R-wrapper of IGD

### 1. Create iGD database from a genome data source
 
```
> library(IGDr)
> createIGD("/path/to/data_source_folder/*" "/path/to/igd_folder/" "databaseName" [option]

where:

- "path/to/data_source_folder/" is the path of the folder that contains .bed.gz or .bed data files.

- "path/to/igd_folder/" is the path to the output igd folder;

- "databaseName" is the name you give to the database, for eaxmple, "roadmap"

option:

-b: bin size in bp (default 16384)
```
### 2. search the igd database in R (an example for a created igd file)

Search the igd database with a single query:
```
> igd_file = "igdr_b14/roadmap.igd"
> library(IGDr)
> igd <- IGDr::IGDr(igd_file)
> hits <- search_1r(igd, "chr6", 1000000, 10000000)
> hits
```
Search the igd database with n queries:
```
> igd_file = "igdr_b14/roadmap.igd"
> library(IGDr)
> igd <- IGDr::IGDr(igd_file)
> chrms = c("chr6", "chr1", "chr2")
> starts = c(10000, 100000, 1000000)
> ends = (100000, 1000000, 10000000)
> hits <- search_nr(igd, 3, chrms, starts, ends)
> hits
```
Search a whole query file chainRn4.bed
```
> igd_file = "igdr_b14/roadmap.igd"
> query_file = "r10000.bed"
> library(bit64)
> library(IGDr)
> fi = IGDr::getFInfo(igd_file)
> hits = integer64(fi$nFiles)
> ret = IGDr::search_all(igd_file, query_file, hits)
> for(i in 1:fi$nFiles){
  cat(i, "\t", toString(ret[i]), "\t", toString(fi$fInfo[i,2]), "\n")
  }
>
```
