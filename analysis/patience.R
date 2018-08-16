

# For Solvitaire experiments

# if not already done then do following
# install.packages("data.table",repos="https://cloud.r-project.org/")
# install.packages("Hmisc",repos="https://cloud.r-project.org/")

require(Hmisc)

# 1 Lower Bound
# 2 Upper Bound
# 3 Solvable
# 4 Unsolvable
# 5 Intractable
# 6 Attempted Seed
# 7 Seed State
# 8 Streamliner Results
# 9 Time Taken(ms)
# 10 States Searched
# 11 Unique States Searched
# 12 Backtracks
# 13 Dominance Moves
# 14 States Removed From Cache
# 15 Final States In Cache
# 16 Final Buckets In Cache
# 17 Maximum Search Depth
# 18 Final Search Depth
# 19+ Seeds In Progress ...

colnames <- c(
              'lowbound', 
              'upbound', 
              'numsolvable',
              'numunsolvable',
              'numintractable',
              'seed',
              'status', 
              'status_str',
              'time',
              'states',
              'states_unique',
              'backtracks',
              'dominances',
              'cache_states_removed',
              'cache_final_states',
              'cache_final_buckets',
              'depth_max',
              'depth_final',
              'index', 
              'sorted_index'
              )

filenames =
     c( "alpha-star.csv",
        "bakers-game.csv",
        "black-hole.csv",
        "castles-of-spain.csv",
        "delta-star.csv",
        "eight-off-any-card-spaces.csv",
        "eight-off.csv",
        "fan.csv",
        "flower-garden.csv",
        "fore-cell.csv",
        "fortunes-favor.csv",
        "free-cell-0-cell.csv",
        "free-cell-1-cell.csv",
        "free-cell-2-cell.csv",
        "free-cell-3-cell.csv",
        "free-cell-4-pile.csv",
        "free-cell-5-pile.csv",
        "free-cell-6-pile.csv",
        "free-cell-7-pile.csv",
        "free-cell.csv",
        "king-albert.csv",
        "northwest-territory.csv",
        "one-cell.csv",
        "penguin.csv",
        "raglan.csv",
        "sea-towers.csv",
        "somerset.csv",
        "spanish-patience.csv",
        "spiderette.csv",
        "two-cell.csv",
        "will-o-the-wisp.csv"
        )


read_stats = function(filename) { 
    maxfields = max(count.fields(filename,sep=","))
    colclasses = c(rep("numeric",6),rep("character",2),rep("numeric",10),rep("NULL",maxfields-18))
    dat = read.csv(filename,skip=4,colClasses=colclasses,header=FALSE,sep=',',strip.white=TRUE,flush=TRUE)
    dat[ncol(dat)+1]=1:nrow(dat)
    colnames(dat)=colnames[1:ncol(dat)]
    odat = dat[order(dat$time),]
    odat[ncol(odat)+1]=1:nrow(odat)
    colnames(odat)=colnames[1:ncol(odat)]
    odat
}

# e.g. dat =read_stats("black-hole.csv") 

dats = lapply(filenames,read_stats) 


plot_patiences = function(fs=filenames,timeout=600000) { 
    d = read_stats(fs[1])
    plot(d$sorted_index/nrow(d) ~ d$time,col=1,ylim=c(0,1),xlim=c(1,timeout),log="x")
    if (length(fs) > 1) { 
        for (i in 2:length(fs)) { 
            d = read_stats(fs[i])
            points(d$sorted_index/nrow(d) ~ d$time,col=i)
        }
    }
}






patience_stats = function(filename,alpha=0.01) { 

    maxfields = max(count.fields(filename,sep=","))
    colclasses = c(rep("numeric",6),rep("character",2),rep("numeric",10),rep("NULL",maxfields-18))
    dat = read.csv(filename,skip=4,colClasses=colclasses,header=FALSE,sep=',',strip.white=TRUE,flush=TRUE)
    colnames(dat)=colnames[1:ncol(dat)]

    num = nrow(dat)
    nsat = nrow(dat[dat$status=="solved",])
    nunsat = nrow(dat[dat$status=="unsolvable",])
    ntimeout = nrow(dat[dat$status=="timed-out",])
    nsat_str = nrow(dat[dat$status_str=="solved",])

    bound1 = binconf(nsat,num,alpha=alpha,method="wilson")
    bound2 = binconf(nsat+ntimeout,num,alpha=0.01,method="wilson")

    est = (bound1[1] + bound2[1])/2
    lbound = bound1[2]
    ubound = bound2[3]

    c(      alpha,
            est,lbound,ubound,
            num,nsat,nunsat,ntimeout,nsat_str,
            median(dat$time),
            mean(dat$time),
            mean(dat$states),
            mean(dat$states_unique),
            mean(dat$backtracks),
            mean(dat$dominances),
            mean(dat$cache_states_removed),
            mean(dat$cache_final_states),
            mean(dat$cache_final_buckets),
            mean(dat$depth_max),
            mean(dat$depth_final)
      )
}

res=t(sapply(filenames,patience_stats))



#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 
#########  IGNORE FROM HERE 








require(data.table)

diags_xlim = c(0,1.75)
diags_ns = c('21','20','19','18','17','16','15','14','13','12','11','10')
diags_types = c(12,11,10,9,8,7,6,5,4,3,2,1)

diags_timelim=c(0.001,200)
diags_meanlim=c(1,2.5e8)


# following for block2dimacs expts 

# n numblocks id status satisfiable timedout conflicts conflicts-p-s decisions decisions-p-s propagations Mprops-p-s lingeling-time total-usertime total-systemtime total-elapsedtime filename

filename <- "run-diags-lingeling-raw.txt"

colnames <- c(
              'n', 'm', 'id',
              'status', 'sat', 'timeout', 
              'conflicts', 'conflictsps',
              'nodes', 'nodesps',
              'props', 'Mpropsps',
              'solvetime',
              'totaltime', 'totalsystime','totalwalltime',
              'filename'
              )

numbercols <- c('totaltime', 'nodes','sat','n','m','conflicts','props','solvetime')



lingelingdata <- read.table(filename,col.names=colnames,header=FALSE)
ld <- data.table(lingelingdata)



ld.out <- ld[,list(x=(m)/n,psat=mean(sat),ptimeout=mean(timeout),nodesmean=mean(nodes),timemean=mean(totaltime),totaltimemean=mean(totaltime+totalsystime),totaltimemedian=median(totaltime+totalsystime),med=as.double(median(nodes)),max=max(nodes)),by=c("n","m")]
xlabel="Number of disallowed diagonals / n"

ld3 <- ld.out[order(n,m)]
plotdata=ld3

pdf("diags-lingeling-prob.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$psat ~ plotdata$x,ann=FALSE,type="n",ylim=c(0,1),xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$psat[plotdata$n==i] ~ plotdata$x[plotdata$n==i],lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Probability solution exists")
title(xlab=xlabel)
grid()
dev.off()

pdf("diags-lingeling-timeoutprob.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$ptimeout ~ plotdata$x,ann=FALSE,type="n",ylim=c(0,1),xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$ptimeout[plotdata$n==i] ~ plotdata$x[plotdata$n==i],lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Probability solution exists")
title(xlab=xlabel)
grid()
dev.off()

pdf("diags-lingeling-mean.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$nodesmean ~ plotdata$x,ann=FALSE,type="n",log='y',ylim=diags_meanlim,xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
#for (i in c(10,15,20,25,30,35,40,45,50,55,60)) {
    lines(plotdata$nodesmean[plotdata$n==i] ~ plotdata$x[plotdata$n==i],
          lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Mean search nodes")
title(xlab=xlabel)
grid()
dev.off()

pdf("diags-lingeling-time.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$timemean ~ plotdata$x,ann=FALSE,type="n",log='y',ylim=diags_timelim,xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$timemean[plotdata$n==i] ~ plotdata$x[plotdata$n==i],
          lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Mean time taken")
title(xlab=xlabel)
grid()
dev.off()

# following for shiftqueens expts 

# n numdiags mininrow id filename numsols text nodes text timeout text time(s)

filename <- "run-diags-shiftqueens-processed.txt" 

colnames <- c(
              'n', 'm', 'mininrow', 'id',
              'filename', 'sat', 
              'c7', 'nodes',
              'c8', 'timeout',
              'c9', 'totaltime'
              )

numbercols <- c('totaltime', 'nodes','sat','n','m','mininrow','timeout')


shiftqueensdata <- read.table(filename,col.names=colnames,header=FALSE)
sd <- data.table(shiftqueensdata)


sd.out <- sd[,list(x=(m)/n,psat=mean(sat),ptimeout=mean(timeout),nodesmean=mean(nodes),timemean=mean(totaltime),totaltimemean=mean(totaltime),totaltimemedian=median(totaltime),med=as.double(median(nodes)),max=max(nodes)),by=c("n","m")]
xlabel="Number of disallowed diagonals / n"

sd3 <- sd.out[order(n,m)]
plotdata=sd3



pdf("diags-shiftqueens-prob.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$psat ~ plotdata$x,ann=FALSE,type="n",ylim=c(0,1),xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$psat[plotdata$n==i] ~ plotdata$x[plotdata$n==i],lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Probability solution exists")
title(xlab=xlabel)
grid()
dev.off()

pdf("diags-shiftqueens-timeoutprob.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$ptimeout ~ plotdata$x,ann=FALSE,type="n",ylim=c(0,1),xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$ptimeout[plotdata$n==i] ~ plotdata$x[plotdata$n==i],lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Probability solution exists")
title(xlab=xlabel)
grid()
dev.off()


pdf("diags-shiftqueens-mean.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$nodesmean ~ plotdata$x,ann=FALSE,type="n",log='y',ylim=diags_meanlim,xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
#for (i in c(10,15,20,25,30,35,40,45,50,55,60)) {
    lines(plotdata$nodesmean[plotdata$n==i] ~ plotdata$x[plotdata$n==i],
          lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Mean search nodes")
title(xlab=xlabel)
grid()
dev.off()

pdf("diags-shiftqueens-time.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$timemean ~ plotdata$x,ann=FALSE,type="n",log='y',ylim=diags_timelim,xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$timemean[plotdata$n==i] ~ plotdata$x[plotdata$n==i],
          lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Mean time taken")
title(xlab=xlabel)
grid()
dev.off()

# following for minion 


filename="run-diags-minion-strong-processed.txt"

# In the stats data the nostats exe has an id appended
# 1 N (size of board)
# 2 M (number of queens to place)
# 3 D (min number in row)
# 4 instance id 
# 5 problem eprime file
# 6 instance param file
# 7 if x cse switched on
# 8 x cse heuristic
# 9 not used
# 10 minion solve time 
# 11 solver total time
# 12 minion setup time
# 13 solver nodes
# 14 solver timeout bool
# 15 solver satisfiable bool
# 16 savile row time
# 17 ?? savile row clauseout
# 18 solver memout bool
# 19 savile row timeout 
# 20 numsat vars
# 21 numsat clauses 

mcolnames <- c('n','m','d','id',
              'eprime', 'param','c3','c4','c5',
              'solvetime','totaltime',
              'setuptime','nodes',
              'timeout', 'sat',
              'srtime','c13',
              'srmemout','srtimeout',
              'numvars','numclauses')

numbercols <- c('totaltime','setuptime','nodes','sat','srtime','n','m','delta')



# colnames.shifttime = c('delta','n','m','id','c5','sat','c7','nodes','c9','timeout','c11','solvetime')

miniondata <- read.table(filename,col.names=mcolnames,header=FALSE)
md <- data.table(miniondata)

md.out <- md[,list(x=(m)/n,psat=mean(sat),ptimeout=mean(timeout),nodesmean=mean(nodes),timemean=mean(totaltime),totaltimemean=mean(totaltime+srtime),totaltimemedian=median(totaltime+srtime),med=as.double(median(nodes)),max=max(nodes)),by=c("n","m")]
md3 <- md.out[order(n,m)]
plotdata=md3

pdf("diags-minion-prob.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$psat ~ plotdata$x,ann=FALSE,type="n",ylim=c(0,1),xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$psat[plotdata$n==i] ~ plotdata$x[plotdata$n==i],lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Probability that solution exists")
title(xlab=xlabel)
grid()
dev.off()

pdf("diags-minion-timeoutprob.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$ptimeout ~ plotdata$x,ann=FALSE,type="n",ylim=c(0,1),xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$ptimeout[plotdata$n==i] ~ plotdata$x[plotdata$n==i],lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Probability solution exists")
title(xlab=xlabel)
grid()
dev.off()

pdf("diags-minion-mean.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$nodesmean ~ plotdata$x,ann=FALSE,type="n",log='y',ylim=diags_meanlim,xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$nodesmean[plotdata$n==i] ~ plotdata$x[plotdata$n==i],
          lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Mean search nodes")
title(xlab=xlabel)
grid()
dev.off()

pdf("diags-minion-time.pdf",width=6,height=6,useDingbats=FALSE)
plot(plotdata$timemean ~ plotdata$x,ann=FALSE,type="n",log='y',ylim=diags_timelim,xlim=diags_xlim)
for (i in c(10,11,12,13,14,15,16,17,18,19,20,21)) {
    lines(plotdata$timemean[plotdata$n==i] ~ plotdata$x[plotdata$n==i],
          lwd=1,lty=i-9,pch=i-9,col=i-9,type='b')
}
legend('topright',diags_ns,lty=diags_types,pch=diags_types,col=diags_types)
title(ylab="Mean time taken")
title(xlab=xlabel)
grid()
dev.off()

quit()

