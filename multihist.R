for(i in 0:9){
	p1 <- hist(diff[diff$cycle == i,]$diff1, breaks=50)
	p2 <- hist(diff[diff$cycle == i,]$diff2, breaks=50)
	p3 <- hist(diff[diff$cycle == i,]$diff3, breaks=50)
	png(paste("simhist", i, ".png",sep=""), width=1280, height=1024)
	plot(p1, col=rgb(0,0,1,1/4), xlim=c(-40, 40), ylim=c(0, 700), main="Histogram")
	plot(p2, col=rgb(1,0,0,1/4), xlim=c(-40, 40), ylim=c(0, 700), add=T)
	plot(p3, col=rgb(0,1,0,1/4), xlim=c(-40, 40), ylim=c(0, 700), add=T)
	dev.off()

	d1 <- data.frame(diff=diff[diff$cycle == i,]$diff1, type="CTCF-Rad21")
	d2 <- data.frame(diff=diff[diff$cycle == i,]$diff2, type="CTCF-Smc3")
	d3 <- data.frame(diff=diff[diff$cycle == i,]$diff3, type="Rad21-Smc3")
	final <- rbind(d1, d2, d3)
	g <- ggplot(final, aes(diff, fill=type)) + geom_density(alpha=0.2)
	ggsave(g, file=paste("simdens", i, ".png", sep=""), width=5, height=3)
}
